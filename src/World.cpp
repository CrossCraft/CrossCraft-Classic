#include "World.hpp"
#include <Platform/Platform.hpp>
#include <Rendering/Rendering.hpp>
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>
#include <iostream>

#if PSP
#include <pspctrl.h>
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if PSP
#include <pspkernel.h>
#else
#include <glad/glad.hpp>
#endif

namespace CrossCraft {
World::World(std::shared_ptr<Player> p) {
    tick_counter = 0;
    player = p;
    pchunk_pos = {-1, -1};

    terrain_atlas = Rendering::TextureManager::get().load_texture(
        "./assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true, false);
    p->terrain_atlas = terrain_atlas;

    fsl.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    fsl.SetFrequency(0.001f * 5.f);
    seed = time(NULL);
    fsl.SetSeed(seed);

    clouds = create_scopeptr<Clouds>();
    psystem = create_scopeptr<ParticleSystem>(terrain_atlas);

    // Zero the array
    worldData =
        reinterpret_cast<block_t *>(calloc(256 * 64 * 256, sizeof(block_t)));
    lightData =
        reinterpret_cast<uint16_t *>(calloc(256 * 4 * 256, sizeof(uint16_t)));

    chunks.clear();

    place_icd = 0.0f;
    break_icd = 0.0f;
}

World::~World() {
    Rendering::TextureManager::get().delete_texture(terrain_atlas);
    free(worldData);
    free(lightData);

    for (auto const &[key, val] : chunks) {
        if (val)
            delete val;
    }
    chunks.clear();
}

#if PSP
const auto RENDER_DISTANCE_DIAMETER = 4.0f;
#else
const auto RENDER_DISTANCE_DIAMETER = 16.f;
#endif

auto World::get_needed_chunks() -> std::vector<glm::ivec2> {
    auto rad = RENDER_DISTANCE_DIAMETER / 2.f;

    std::vector<glm::ivec2> needed_chunks;
    for (auto x = -rad; x < rad; x++) {
        for (auto y = -rad; y < rad; y++) {
            // Now bound check
            auto bx = x + pchunk_pos.x;
            auto by = y + pchunk_pos.y;

            if (bx >= 0 && bx < 16 && by >= 0 && by < 16) {
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

    if (tick_counter > 0.25) {
        tick_counter = 0;

        for (auto &[key, value] : chunks) {
            // Random tick
            value->rtick_update(this);

            // Chunk Updates
            value->chunk_update(this);
        }

        for (auto &[key, value] : chunks) {
            value->post_update(this);
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
            ChunkStack *stack = new ChunkStack(ipos.x, ipos.y);
            stack->generate(this);

            uint16_t x = ipos.x;
            uint16_t y = ipos.y;
            uint32_t id = x << 16 | (y & 0x00FF);
            chunks.emplace(id, stack);
        }
    }
}
/**
 * @brief Remap floats into a range
 *
 * @param input To be remapped
 * @param curr_range_min Current Min
 * @param curr_range_max Current Max
 * @param range_min New Range Min
 * @param range_max New Range Max
 */
inline auto range_map(float &input, float curr_range_min, float curr_range_max,
                      float range_min, float range_max) -> void {
    input = (input - curr_range_min) * (range_max - range_min) /
                (curr_range_max - curr_range_min) +
            range_min;
}

auto World::get_noise(float x, float y, NoiseSettings *settings) -> float {

    float amp = settings->amplitude;
    float freq = settings->frequency;

    float sum_noise = 0.0f;
    float sum_amp = 0.0f;

    // Create octaves
    for (auto i = 0; i < settings->octaves; i++) {
        auto noise = fsl.GetNoise(x * freq, y * freq);

        noise *= amp;
        sum_noise += noise;
        sum_amp += amp;

        amp *= settings->persistence;
        freq *= settings->mod_freq;
    }

    // Reset range
    auto divided = sum_noise / sum_amp;

    // Map to the new range;
    range_map(divided, -1.0f, 1.0f, settings->range_min, settings->range_max);

    return divided;
}

auto World::getIdx(int x, int y, int z) -> uint32_t {
    if (x < 0 || x >= 256 || y >= 64 || y < 0 || z < 0 || z >= 256)
        return 0;
    return (x * 256 * 64) + (z * 64) + y;
}

auto World::generate_tree(int x, int z, int hash) -> void {
    x *= 8;
    z *= 8;

    x += rand() % 5 - 2;
    z += rand() % 5 - 2;

    if (!(x >= 0 && x < 256 && z >= 0 && z < 256))
        return;

    int h = hmap[x * 256 + z] * 64.f;

    if (h < 33)
        return;

    int tree_height = rand() % 3 + 4;

    for (int i = 0; i < tree_height + 1; i++) {
        // lower layer
        if (i > tree_height - 4 && i < tree_height - 1) {
            for (int tx = -2; tx < 3; tx++)
                for (int tz = -2; tz < 3; tz++)
                    worldData[getIdx(x + tx, h + i, z + tz)] = 18;
        } else if (i >= tree_height - 1 && i < tree_height) {
            for (int tx = -1; tx < 2; tx++)
                for (int tz = -1; tz < 2; tz++)
                    worldData[getIdx(x + tx, h + i, z + tz)] = 18;
        } else if (i == tree_height) {
            worldData[getIdx(x - 1, h + i, z)] = 18;
            worldData[getIdx(x + 1, h + i, z)] = 18;
            worldData[getIdx(x, h + i, z)] = 18;
            worldData[getIdx(x, h + i, z + 1)] = 18;
            worldData[getIdx(x, h + i, z - 1)] = 18;
        }

        // write the log
        if (i < tree_height) {
            worldData[getIdx(x, h + i, z)] = 17;
        }
    }
}

void World::generate() {
    // Create a height map
    hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

    NoiseSettings settings = {2, 1.0f, 2.0f, 0.42f, 4.5f, 0.0f, 0.15f, 0.85f};

    // Generate HMAP
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            hmap[x * 256 + z] = get_noise(x, z, &settings);
        }
    }

    // Generate world data / fill data
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            int h = hmap[x * 256 + z] * 64.f;
            for (int y = 0; y < h; y++) {
                auto idx = (x * 256 * 64) + (z * 64) + y;
                if (y < (h - 4))
                    worldData[idx] = 1;
                else if (y >= (h - 4) && y < (h - 1))
                    worldData[idx] = 2;
                else {
                    if (h < 32)
                        worldData[idx] = 2;
                    else if (h == 32)
                        worldData[idx] = 12;
                    else
                        worldData[idx] = 3;
                }
            }

            if (h < 32) {
                for (int y = h; y < 32; y++) {
                    auto idx = (x * 256 * 64) + (z * 64) + y;
                    worldData[idx] = 8;
                }
            }
        }
    }

    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {

            srand((x | 1) << z * ~seed);
            uint8_t res = rand() % 37;

            if (res < 2) {
                int xf = x * 8 + rand() % 5 - 2;
                int zf = z * 8 + rand() % 5 - 2;
                if (xf < 0 || xf >= 256 || zf < 0 || zf >= 256)
                    continue;

                int h = hmap[xf * 256 + zf] * 64.f;
                if ((h + 1) <= 33)
                    continue;

                auto idx = (xf * 256 * 64) + (zf * 64) + h;

                worldData[idx] = 37 + res % 2;

                continue;
            } else if (res < 6)
                continue;

            generate_tree(x, z, res);
        }
    }

    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            auto idx = (x * 256 * 64) + (z * 64) + 0;

            worldData[idx] = 7;
        }
    }

    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            for (int y = 63; y >= 0; y--) {
                auto idx = (x * 256 * 64) + (z * 64) + y;
                auto blk = worldData[idx];
                if (blk == 0 || blk == 37 || blk == 38 || blk == 39 ||
                    blk == 40 || blk == 20 || blk == 6)
                    continue;

                auto idx2 = (x * 256 * 4) + (z * 4) + y / 16;
                lightData[idx2] |= 1 << (y % 16);
                break;
            }
        }
    }

    // Destroy height map
    free(hmap);

    player->spawn(this);
}

void World::draw() {

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

#if !BUILD_PC
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_ALPHA_TEST);
#endif

    // Draw opaque
    for (auto const &[key, val] : chunks) {
        val->draw();
    }

#if !BUILD_PC
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_ALPHA_TEST);
#endif

    // Draw transparent
    for (auto const &[key, val] : chunks) {
        val->draw_transparent();
    }

    clouds->draw();
    psystem->draw();

    player->draw();
}

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

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
        auto idx = (x * 256 * 64) + (z * 64) + y;
        auto blk = worldData[idx];
        if (blk == 0 || blk == 37 || blk == 20 || blk == 38 || blk == 39 ||
            blk == 40 || blk == 6)
            continue;

        auto idx2 = (x * 256 * 4) + (z * 4) + y / 16;
        lightData[idx2] |= 1 << (y % 16);
        break;
    }
}

auto validate_ivec3(glm::ivec3 ivec) -> bool {
    return ivec.x >= 0 && ivec.x < 256 && ivec.y >= 0 && ivec.y < 256 &&
           ivec.z >= 0 && ivec.z < 256;
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
    add_update({ivec.x, ivec.y, ivec.z });
    add_update({ivec.x, ivec.y + 1, ivec.z});
    add_update({ivec.x, ivec.y - 1, ivec.z});
    add_update({ivec.x - 1, ivec.y, ivec.z});
    add_update({ivec.x + 1, ivec.y, ivec.z});
    add_update({ivec.x, ivec.y, ivec.z + 1});
    add_update({ivec.x, ivec.y, ivec.z - 1});
}

auto World::dig(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

    if (w->break_icd < 0)
        w->break_icd = 0.2f;
    else
        return;

    auto pos = w->player->get_pos();
    auto default_vec = glm::vec3(0, 0, 1);

    if (w->player->in_inventory) {
        using namespace Stardust_Celeste::Utilities;
        auto cX = (Input::get_axis("Mouse", "X") + 1.0f) / 2.0f;
        auto cY = (Input::get_axis("Mouse", "Y") + 1.0f) / 2.0f;

#if PSP
        cX = w->player->vcursor_x / 480.0f;
        cY = 1.0f - (w->player->vcursor_y / 272.0f);
#endif

        if (cX > 0.3125f && cX < 0.675f)
            cX = (cX - 0.3125f) / .04f;
        else
            return;

        if (cY > 0.3125f && cY < 0.7188f)
            cY = (cY - 0.3125f) / .08f;
        else
            return;

        int iX = cX;
        int iY = cY;

        int idx = iY * 9 + iX;

        if (idx > 41)
            return;

        w->player->itemSelections[w->player->selectorIDX] =
            w->player->inventorySelection[idx];

        return;
    }

    default_vec = glm::rotateX(default_vec, DEGTORAD(w->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-w->player->get_rot().y + 180));

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

        if (!validate_ivec3(ivec))
            continue;

        u32 idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;
        auto blk = w->worldData[idx];

        if (blk == 0 || blk == 7 || blk == 8)
            continue;

        w->psystem->initialize(blk, cast_pos);

        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

        w->worldData[idx] = 0;

        // Update Lighting
        w->update_lighting(ivec.x, ivec.z);

        if (w->chunks.find(id) != w->chunks.end())
            w->chunks[id]->generate(w);

        w->update_surroundings(ivec.x, ivec.z);
        w->update_nearby_blocks(ivec);

        break;
    }
}

auto World::place(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

    if (w->place_icd < 0)
        w->place_icd = 0.2f;
    else
        return;

    auto pos = w->player->get_pos();

    if (w->player->in_inventory)
        return;

    auto pos_ivec = glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y),
                               static_cast<s32>(pos.z));

    if (!validate_ivec3(pos_ivec))
        return;

    auto pidx = (pos_ivec.x * 256 * 64) + (pos_ivec.z * 64) + pos_ivec.y;
    if (w->worldData[pidx] != 0 && w->worldData[pidx] != 8)
        return;

    auto default_vec = glm::vec3(0, 0, 1);

    default_vec = glm::rotateX(default_vec, DEGTORAD(w->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-w->player->get_rot().y + 180));

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

        auto posivec =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y),
                       static_cast<s32>(pos.z));
        auto posivec2 =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y - 1),
                       static_cast<s32>(pos.z));
        auto posivec3 =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y - 1.8f),
                       static_cast<s32>(pos.z));

        if (!validate_ivec3(ivec) || ivec == posivec)
            continue;

        u32 idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;
        auto blk = w->worldData[idx];

        if (blk == 0 || blk == 8)
            continue;

        cast_pos = pos + (default_vec * static_cast<float>(i - 1) /
                          static_cast<float>(NUM_STEPS));

        ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                          static_cast<s32>(cast_pos.y),
                          static_cast<s32>(cast_pos.z));

        if (!validate_ivec3(ivec))
            return;

        auto bk = w->player->itemSelections[w->player->selectorIDX];
        if ((ivec == posivec || ivec == posivec2 || ivec == posivec3) &&
            (bk != 6 && bk != 37 && bk != 38 && bk != 39 && bk != 40))
            return;

        idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;


        auto idx2 = (ivec.x * 256 * 64) + (ivec.z * 64) + (ivec.y - 1);

        
        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);
        
        blk = w->player->itemSelections[w->player->selectorIDX];

        auto blk2 = w->worldData[idx2];

        if ((blk == 37 || blk == 38) && blk2 != 3)
            return;

        if (blk == 6 && (blk2 != 3 && blk2 != 2))
            return;

        if ((blk == 39 || blk == 40) && (blk2 != 1 && blk2 != 4 && blk2 != 13))
            return;

        w->worldData[idx] = blk;

        // Update Lighting
        w->update_lighting(ivec.x, ivec.z);

        if (w->chunks.find(id) != w->chunks.end())
            w->chunks[id]->generate(w);

        w->update_surroundings(ivec.x, ivec.z);
        w->update_nearby_blocks(ivec);

        return;
        break;
    }
}

} // namespace CrossCraft