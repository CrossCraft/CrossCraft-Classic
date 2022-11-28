#include "World.hpp"
#include "../Modding/Mod.hpp"
#include "../TexturePackManager.hpp"
#include "Generation/NoiseUtil.hpp"
#include "Generation/WorldGenUtil.hpp"
#include "SelectionBox.hpp"
#include <Platform/Platform.hpp>
#include <Rendering/Rendering.hpp>
#include <Utilities/Input.hpp>
#include <iostream>

#if PSP
#include <pspctrl.h>
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if PSP
#include <pspkernel.h>
#elif BUILD_PLAT == BUILD_VITA
#include <vitaGL.h>
#elif BUILD_PLAT == BUILD_PS2
#include <GL/gl.h>
#else
#include <glad/glad.hpp>
#endif

namespace CrossCraft {

World::World(std::shared_ptr<Player> p) {
    tick_counter = 0;
    player = p;
    pchunk_pos = {-1, -1};
    world_size = {256, 64, 256};
    hmap = nullptr;
    client = nullptr;

    terrain_atlas = TexturePackManager::get().load_texture(
        "assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true, false, true);
    p->terrain_atlas = terrain_atlas;
    p->wrldRef = this;

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
    chunksMeta = reinterpret_cast<ChunkMeta *>(
        calloc((uint64_t)world_size.x / 16 * (uint64_t)world_size.y / 16 *
                   (uint64_t)world_size.z / 16,
               sizeof(ChunkMeta)));

    chunks.clear();

    sbox = create_scopeptr<SelectionBox>();

    place_icd = 0.0f;
    break_icd = 0.0f;
    chunk_generate_icd = 0.0f;
}

auto World::spawn() -> void { player->spawn(this); }
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto World::generate_meta() -> void {
    for (int x = 0; x < world_size.x / 16; x++)
        for (int y = 0; y < world_size.y / 16; y++)
            for (int z = 0; z < world_size.z / 16; z++) {
                int idx = y * world_size.x / 16 * world_size.z / 16 +
                          z * world_size.x / 16 + x;

                auto &meta = chunksMeta[idx];
                meta.is_empty = true;
                meta.is_full = true;

                for (int layer = 0; layer < 16; layer++) {
                    meta.layers[layer].is_empty = true;
                    meta.layers[layer].is_empty = false;
                }

                for (int local_x = 0; local_x < 16; local_x++)
                    for (int local_z = 0; local_z < 16; local_z++)
                        for (int local_y = 0; local_y < 16; local_y++) {
                            auto cx = x * 16 + local_x;
                            auto cy = y * 16 + local_y;
                            auto cz = z * 16 + local_z;

                            auto idx = getIdx(cx, cy, cz);

                            auto blk = worldData[idx];

                            if (blk != Block::Air) {
                                meta.is_empty = false;
                                meta.layers[local_y].is_empty = false;
                            }
                            if (blk == Block::Air || blk == Block::Water ||
                                blk == Block::Sapling || blk == Block::Leaves ||
                                blk == Block::Glass || blk == Block::Slab ||
                                blk == Block::Flower1 ||
                                blk == Block::Flower2 ||
                                blk == Block::Mushroom1 ||
                                blk == Block::Mushroom2) {
                                meta.is_full = false;
                                meta.layers[local_y].is_full = false;
                            }
                        }
            }
}

World::~World() {
    Rendering::TextureManager::get().delete_texture(terrain_atlas);
    free(worldData);
    free(lightData);
    free(chunksMeta);
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
const auto CHUNKS_PER_SECOND = 2.0f;
#elif BUILD_PLAT == BUILD_VITA
const auto CHUNKS_PER_SECOND = 4.0f;
#else
const auto CHUNKS_PER_SECOND = 96.0f;
#endif

#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA
const auto RENDER_DISTANCE_DIAMETER = 10.0f;
#else
const auto RENDER_DISTANCE_DIAMETER = 16.f;
#endif

auto World::get_needed_chunks() -> std::vector<glm::ivec2> {
    auto RDIST_DIAMETER = RENDER_DISTANCE_DIAMETER *
                          static_cast<float>(Option::get().renderDist + 1) /
                          4.0f;

    auto rad = RDIST_DIAMETER / 2.f;

    std::vector<glm::ivec2> needed_chunks;
    for (auto x = (-rad - 1); x < (rad + 1); x++) {
        for (auto y = (-rad - 1); y < (rad + 1); y++) {
            auto bx = x + pchunk_pos.x;
            auto by = y + pchunk_pos.y;

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

int old_rdist = 0;
void World::update(double dt) {

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();
    player->update(static_cast<float>(dt), this);
    sbox->update_position(this);
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

                value->post_update(this);
            }

            Modding::ModManager::get().onTick();
        }
    }

    auto ppos = player->get_pos();
    glm::ivec2 new_pos = {static_cast<int>(ppos.x) / 16,
                          static_cast<int>(ppos.z) / 16};

    if (pchunk_pos != new_pos || Option::get().renderDist != old_rdist) {
        old_rdist = Option::get().renderDist;
        pchunk_pos = new_pos;

        // Get what's needed
        auto needed = get_needed_chunks();

        // Check what we have - what we still need

        std::map<int, ChunkStack *> existing_chunks;

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

                glm::vec2 appos = {ppos.x / 16.0f, ppos.z / 16.0f};
                glm::vec2 aipos = {(float)ipos.x, (float)ipos.y};

                glm::vec2 diff = appos - aipos;
                float len = diff.x * diff.x + diff.y * diff.y;
                to_generate.emplace(len, ipos);
            }
        }

        // Remaining can be released
        for (auto &[key, value] : chunks) {
            delete value;
        }
        chunks.clear();

        // Now merge existing into blank map
        chunks.merge(existing_chunks);
    }

    chunk_generate_icd -= dt;

    // Generate remaining
    if (chunk_generate_icd <= 0.0f) {
        chunk_generate_icd = 1.0f / (float)CHUNKS_PER_SECOND;

        if (to_generate.size() > 0) {
            auto ipos = to_generate.begin()->second;

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

            to_generate.erase(to_generate.begin()->first);
        }
    }
}

void World::draw() {

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

#if BUILD_PLAT == BUILD_PSP
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_ALPHA_TEST);
    sceGuEnable(GU_FOG);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFog(0.2f * 2.5f * 16.0f, 0.8f * 2.5f * 16.0f, 0x00FFCC99);
#else
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#endif

    std::map<float, ChunkStack *> chunk_sorted;
    chunk_sorted.clear();

    for (auto const &[key, val] : chunks) {
        glm::vec2 relative_chunk_pos = glm::vec2(
            val->get_chunk_pos().x * 16.0f, val->get_chunk_pos().y * 16.0f);
        auto diff =
            glm::vec2(player->pos.x, player->pos.z) - relative_chunk_pos;
        auto len = fabsf(sqrtf(diff.x * diff.x + diff.y * diff.y));
        chunk_sorted.emplace(len, val);
    }

    // Draw opaque
    for (auto const &[key, val] : chunk_sorted) {
        val->draw(this);
    }

    std::map<float, ChunkStack *, std::greater<float>> chunk_reverse_sorted;
    chunk_reverse_sorted.insert(chunk_sorted.begin(), chunk_sorted.end());
    chunk_sorted.clear();

#if BUILD_PLAT == BUILD_VITA
    glEnable(GL_BLEND);
#elif BUILD_PLAT == BUILD_PSP
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_ALPHA_TEST);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    // Draw transparent
    for (auto const &[key, val] : chunk_reverse_sorted) {
        val->draw_transparent();
    }

#if BUILD_PLAT == BUILD_PSP
    sceGuDisable(GU_FOG);
#endif

    sbox->draw();

    clouds->draw();
    psystem->draw(glm::vec3(player->rot, 0.0f));

    player->draw(this);
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
        int idx2 = getIdxl(x, i * 16, z);
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
    if (!validate_ivec3(ivec, world_size))
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
