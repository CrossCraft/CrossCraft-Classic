#include "World.hpp"
#include "../TexturePackManager.hpp"
#include "Generation/NoiseUtil.hpp"
#include "Generation/WorldGenUtil.hpp"
#include <Platform/Platform.hpp>
#include <Rendering/Rendering.hpp>
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>
#include <iostream>
#include <zlib.h>

#if PSP
#include <pspctrl.h>
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if PSP
#include <pspkernel.h>
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#else
#include <glad/glad.hpp>
#endif

namespace CrossCraft {

auto World::add_face_to_mesh(std::array<float, 12> data,
                             std::array<float, 8> uv, uint32_t lightVal,
                             glm::vec3 mypos) -> void { // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {
        m_verts.push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            data[idx++] + mypos.x,
            data[idx++] + mypos.y,
            data[idx++] + mypos.z,
        });
    }

    // Push Back Indices
    m_index.push_back(idx_counter);
    m_index.push_back(idx_counter + 1);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 2);
    m_index.push_back(idx_counter + 3);
    m_index.push_back(idx_counter + 0);
    idx_counter += 4;
}

World::World(std::shared_ptr<Player> p) {
    tick_counter = 0;
    player = p;
    pchunk_pos = {-1, -1};
    world_size = {256, 64, 256};
    hmap = nullptr;
    client = nullptr;

    terrain_atlas = TexturePackManager::get().load_texture(
        "assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true, false);
    p->terrain_atlas = terrain_atlas;

    NoiseUtil::initialize();

    clouds = create_scopeptr<Clouds>();
    psystem = create_scopeptr<ParticleSystem>(terrain_atlas);

    // Zero the array
    worldData = reinterpret_cast<block_t *>(
        calloc((uint64_t)world_size.x * (uint64_t)world_size.y *
                   (uint64_t)world_size.z,
               sizeof(block_t)));
    lightData = reinterpret_cast<uint16_t *>(
        calloc((uint64_t)world_size.x * ((uint64_t)world_size.y / 16) *
                   (uint64_t)world_size.z,
               sizeof(uint16_t)));

    chunks.clear();

    place_icd = 0.0f;
    break_icd = 0.0f;

    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();
    blockMesh.delete_data();

    add_face_to_mesh(bottomFace, getTexCoord(96, LIGHT_BOT), LIGHT_BOT,
                     {0, 0, 0});
    add_face_to_mesh(topFace, getTexCoord(96, LIGHT_TOP), LIGHT_TOP, {0, 0, 0});
    add_face_to_mesh(frontFace, getTexCoord(96, LIGHT_SIDE_Z), LIGHT_SIDE_Z,
                     {0, 0, 0});
    add_face_to_mesh(backFace, getTexCoord(96, LIGHT_SIDE_Z), LIGHT_SIDE_Z,
                     {0, 0, 0});
    add_face_to_mesh(leftFace, getTexCoord(96, LIGHT_SIDE_X), LIGHT_SIDE_X,
                     {0, 0, 0});
    add_face_to_mesh(rightFace, getTexCoord(96, LIGHT_SIDE_X), LIGHT_SIDE_X,
                     {0, 0, 0});
    add_face_to_mesh(leftFace, getTexCoord(96, LIGHT_SIDE_X), LIGHT_SIDE_X,
                     {0, 0, 1});
    add_face_to_mesh(rightFace, getTexCoord(96, LIGHT_SIDE_X), LIGHT_SIDE_X,
                     {0, 0, 1});

    blockMesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                       idx_counter);
}

auto World::spawn() -> void { player->spawn(this); }

auto World::load_world() -> bool {
    gzFile save_file = gzopen("save.ccc", "rb");
    gzrewind(save_file);

    int version = 0;
    gzread(save_file, &version, sizeof(int) * 1);

    SC_APP_DEBUG("READING FILE -- VERSION {}", version);

    if (version != 1)
        return false;

    gzread(save_file, worldData, 256 * 64 * 256);
    gzclose(save_file);

    // Update Lighting
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            update_lighting(x, z);
        }
    }

    return true;
}

auto World::save(std::any p) -> void {
    auto wrld = std::any_cast<World *>(p);
    if (wrld->client == nullptr) {
        SC_APP_DEBUG("SAVING!");

        gzFile save_file = gzopen("save.ccc", "wb");
        if (save_file != nullptr) {
            const int save_version = 1;
            gzwrite(save_file, &save_version, 1 * sizeof(int));
            gzwrite(save_file, wrld->worldData, 256 * 64 * 256);

            gzclose(save_file);
        }
    }
}

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto World::draw_selection() -> void {
    auto pos = player->get_pos();
    auto default_vec = glm::vec3(0, 0, 1);
    default_vec = glm::rotateX(default_vec, DEGTORAD(player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-player->get_rot().y + 180));

    const float REACH_DISTANCE = 4.0f;
    default_vec *= REACH_DISTANCE;

    const u32 NUM_STEPS = 50;

    for (u32 i = 0; i < NUM_STEPS; i++) {
        float percentage =
            static_cast<float>(i) / static_cast<float>(NUM_STEPS);

        auto cast_pos = pos + (default_vec * percentage);

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        u32 idx = getIdx(ivec.x, ivec.y, ivec.z);
        if (idx < 0)
            return;

        auto blk = worldData[idx];

        if (blk == Block::Air || blk == Block::Bedrock || blk == Block::Water ||
            blk == Block::Lava)
            continue;

        if (ivec.x < 0 || ivec.x > 255 || ivec.y < 0 || ivec.y > 255 ||
            ivec.z < 0 || ivec.z > 255)
            return;

        auto ctx = &Rendering::RenderContext::get();

        ctx->matrix_translate(glm::vec3(ivec.x, ivec.y, ivec.z));
        ctx->matrix_rotate({0, 0, 0});
        ctx->matrix_scale({1.01f, 1.01f, 1.01f});
        ctx->matrix_translate({-0.005f, -0.005f, -0.005f});

        blockMesh.draw_wireframe();

        ctx->matrix_clear();
        ctx->matrix_translate(glm::vec3(ivec.x, ivec.y, ivec.z));
        ctx->matrix_rotate({90, 0, 0});
        ctx->matrix_scale({1.01f, 1.01f, 1.01f});
        ctx->matrix_translate({-0.005f, -0.005f, -0.005f - 1.0f});

        blockMesh.draw_wireframe();

        ctx->matrix_clear();
        ctx->matrix_translate(glm::vec3(ivec.x, ivec.y, ivec.z));
        ctx->matrix_rotate({0, 90, 0});
        ctx->matrix_scale({1.01f, 1.01f, 1.01f});
        ctx->matrix_translate({-0.005f - 1.0f, -0.005f, -0.005f});

        blockMesh.draw_wireframe();

        ctx->matrix_clear();
        ctx->matrix_translate(glm::vec3(ivec.x, ivec.y, ivec.z));
        ctx->matrix_rotate({0, 0, 90});
        ctx->matrix_scale({1.01f, 1.01f, 1.01f});
        ctx->matrix_translate({-0.005f, -0.005f - 1.0f, -0.005f});

        blockMesh.draw_wireframe();

        ctx->matrix_clear();
        return;
    }
}

World::~World() {
    Rendering::TextureManager::get().delete_texture(terrain_atlas);
    free(worldData);
    free(lightData);
    // Destroy height map
    if (hmap != nullptr)
        free(hmap);

    for (auto const &[key, val] : chunks) {
        if (val)
            delete val;
    }
    chunks.clear();
}

#if PSP
const auto RENDER_DISTANCE_DIAMETER = 4.0f;
#elif BUILD_PLAT == BUILD_VITA
const auto RENDER_DISTANCE_DIAMETER = 6.0f;
#else
const auto RENDER_DISTANCE_DIAMETER = 12.f;
#endif

auto World::get_needed_chunks() -> std::vector<glm::ivec2> {
    auto rad = RENDER_DISTANCE_DIAMETER / 2.f;

    std::vector<glm::ivec2> needed_chunks;
    for (auto x = -rad; x < rad; x++) {
        for (auto y = -rad; y < rad; y++) {
            // Now bound check
            auto bx = x + pchunk_pos.x;
            auto by = y + pchunk_pos.y;

            // Okay - this is in bounds - now, we check if in radius
            auto dx = (bx - pchunk_pos.x);
            dx *= dx;

            auto dy = (by - pchunk_pos.y);
            dy *= dy;

            auto d = dx + dy;

            // If distance <= radius
            if (d <= rad * rad) {
                needed_chunks.push_back({bx, by});
            }
        }
    }
    return needed_chunks;
}

void World::update(double dt) {

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();
    player->update(static_cast<float>(dt), this);
    clouds->update(dt);
    psystem->update(dt);

    tick_counter += dt;

    break_icd -= dt;
    place_icd -= dt;
    if (client == nullptr) {
        if (tick_counter > 0.5f) {
            tick_counter = 0;

            for (auto &[key, value] : chunks) {
                // Random tick
                for (int i = 0; i < 30; i++)
                    value->rtick_update(this);

                // Chunk Updates
                value->chunk_update(this);
            }

            for (auto &[key, value] : chunks) {
                value->post_update(this);
            }
        }
    }

    auto ppos = player->get_pos();
    glm::ivec2 new_pos = {static_cast<int>(ppos.x) / 16,
                          static_cast<int>(ppos.z) / 16};

    if (pchunk_pos != new_pos) {
        pchunk_pos = new_pos;

        // Get what's needed
        auto needed = get_needed_chunks();

        // Check what we have - what we still need

        std::map<int, ChunkStack *> existing_chunks;
        std::vector<glm::ivec2> to_generate;

        for (auto &ipos : needed) {
            uint16_t x = ipos.x;
            uint16_t y = ipos.y;
            uint32_t id = x << 16 | (y & 0x00FF);

            if (chunks.find(id) != chunks.end()) {
                // move to existing
                existing_chunks.emplace(id, chunks[id]);
                chunks.erase(id);
            } else {
                // needs generated
                to_generate.push_back(ipos);
            }
        }

        // Remaining can be released
        for (auto &[key, value] : chunks) {
            delete value;
        }
        chunks.clear();

        // Now merge existing into blank map
        chunks.merge(existing_chunks);

        // Generate remaining
        for (auto &ipos : to_generate) {
            if (ipos.x >= 0 && ipos.x < (world_size.x / 16) && ipos.y >= 0 &&
                ipos.y < (world_size.z / 16)) {
                ChunkStack *stack = new ChunkStack(ipos.x, ipos.y);
                stack->generate(this);

                uint16_t x = ipos.x;
                uint16_t y = ipos.y;
                uint32_t id = x << 16 | (y & 0x00FF);
                chunks.emplace(id, stack);
            } else if (cfg.compat) {
                ChunkStack *stack = new ChunkStack(ipos.x, ipos.y);
                stack->generate_border();

                uint16_t x = ipos.x;
                uint16_t y = ipos.y;
                uint32_t id = x << 16 | (y & 0x00FF);
                chunks.emplace(id, stack);
            }
        }
    }
}

auto World::getIdx(int x, int y, int z) -> uint32_t const {
    if (x < 0 || x >= world_size.x || y >= world_size.y || y < 0 || z < 0 ||
        z >= world_size.z)
        return 0;
    return (x * world_size.z * world_size.y) + (z * world_size.y) + y;
}

auto World::getIdxl(int x, int y, int z) -> uint32_t const {
    if (x < 0 || x >= world_size.x || y >= world_size.y || y < 0 || z < 0 ||
        z >= world_size.z)
        return 0;
    return (x * world_size.z * (world_size.y / 16)) +
           (z * (world_size.y / 16)) + y / 16;
}

void World::draw() {

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

#if BUILD_PLAT == BUILD_PSP
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_ALPHA_TEST);
    sceGuEnable(GU_FOG);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFog(12.0f, 32.0f, 0x00FFCCCC);
#elif BUILD_PLAT == BUILD_VITA
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FOG);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 12.0f);
    glFogf(GL_FOG_END, 48.0f);
    const float FOG_COLOR[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glFogfv(GL_FOG_COLOR, FOG_COLOR);
#endif

    // Draw opaque
    for (auto const &[key, val] : chunks) {

        glm::vec2 front = {0.0f, 1.0f};
        front = glm::rotate(front, player->cam.rot.y);

        glm::vec2 relative_chunk_pos =
            glm::vec2(val->get_chunk_pos().x * 16.0f,
                      val->get_chunk_pos().y * 16.0f) -
            glm::vec2(player->pos.x, player->pos.z);

        float dot =
            front.x * relative_chunk_pos.x + front.y * relative_chunk_pos.y;

        float x = dot / (front.length() * relative_chunk_pos.length());

        float angle = (x >= -1 && x <= 1) ? abs(acosf(x)) : 0.0f;

        if (angle < DEGTORAD(100.0f) || relative_chunk_pos.length() < 32.0f)
            val->draw();
    }

#if BUILD_PLAT == BUILD_PSP
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_ALPHA_TEST);
#elif BUILD_PLAT == BUILD_VITA
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
#endif

    draw_selection();

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    // Draw flora
    for (auto const &[key, val] : chunks) {
        val->draw_flora();
    }

    // Draw transparent
    for (auto const &[key, val] : chunks) {
        if (!val->border)
            val->draw_transparent();
    }

    // Draw transparent
    for (auto const &[key, val] : chunks) {
        if (val->border)
            val->draw_transparent();
    }

#if BUILD_PLAT == BUILD_PSP
    sceGuDisable(GU_FOG);
#elif BUILD_PLAT == BUILD_VITA
    glDisable(GL_FOG);
#endif

    clouds->draw();
    psystem->draw();

    player->draw();
}

auto World::update_surroundings(int x, int z) -> void {
    auto localX = x % 16;
    auto localZ = z % 16;

    uint16_t cX = x / 16;
    uint16_t cY = z / 16;

    bool xMod = true;
    auto nX = cX;
    if (localX == 0) {
        nX -= 1;
    } else if (localX == 15) {
        nX += 1;
    } else {
        xMod = false;
    }

    if (xMod && nX >= 0 && nX < 16) {
        uint32_t idxx = nX << 16 | (cY & 0x00FF);
        if (chunks.find(idxx) != chunks.end())
            chunks[idxx]->generate(this);
    }

    bool zMod = true;
    auto nY = cY;
    if (localZ == 0) {
        nY -= 1;
    } else if (localZ == 15) {
        nY += 1;
    } else {
        zMod = false;
    }

    if (zMod && nY >= 0 && nY < 16) {
        uint32_t idzz = 0 | cX << 16 | (nY & 0x00FF);
        if (chunks.find(idzz) != chunks.end())
            chunks[idzz]->generate(this);
    }
}

auto World::update_lighting(int x, int z) -> void {
    // Clear
    for (int i = 0; i < 4; i++) {
        auto idx2 = (x * 256 * 4) + (z * 4) + i;
        lightData[idx2] = 0;
    }

    // Retrace
    for (int y = 63; y >= 0; y--) {
        auto idx = getIdx(x, y, z);
        auto blk = worldData[idx];
        if (blk == Block::Air || blk == Block::Flower1 ||
            blk == Block::Flower2 || blk == Block::Mushroom1 ||
            blk == Block::Mushroom2 || blk == Block::Sapling ||
            blk == Block::Glass || blk == Block::Leaves) {
            auto idx2 = getIdxl(x, y, z);
            lightData[idx2] |= 1 << (y % 16);
            continue;
        }

        auto idx2 = getIdxl(x, y, z);
        lightData[idx2] |= 1 << (y % 16);
        break;
    }
}

auto World::set_block(short x, short y, short z, uint8_t mode, uint8_t block)
    -> void {
    client->set_block(x, y, z, mode, block);
}

auto World::add_update(glm::ivec3 ivec) -> void {
    if (!validate_ivec3(ivec))
        return;

    uint16_t x = ivec.x / 16;
    uint16_t y = ivec.z / 16;
    uint32_t id = x << 16 | (y & 0x00FF);

    if (chunks.find(id) != chunks.end()) {
        chunks[id]->posUpdate.push_back(ivec);
    }
}

auto World::update_nearby_blocks(glm::ivec3 ivec) -> void {
    add_update({ivec.x, ivec.y, ivec.z});
    add_update({ivec.x, ivec.y + 1, ivec.z});
    add_update({ivec.x, ivec.y - 1, ivec.z});
    add_update({ivec.x - 1, ivec.y, ivec.z});
    add_update({ivec.x + 1, ivec.y, ivec.z});
    add_update({ivec.x, ivec.y, ivec.z + 1});
    add_update({ivec.x, ivec.y, ivec.z - 1});
}

} // namespace CrossCraft
