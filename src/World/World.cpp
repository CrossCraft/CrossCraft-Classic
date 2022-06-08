#include "World.hpp"
#include "NoiseUtil.hpp"
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

    NoiseUtil::initialize();

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
    // Destroy height map
    free(hmap);

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

    if (tick_counter > 0.15) {
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

auto World::getIdx(int x, int y, int z) -> uint32_t {
    if (x < 0 || x >= 256 || y >= 64 || y < 0 || z < 0 || z >= 256)
        return 0;
    return (x * 256 * 64) + (z * 64) + y;
}

auto World::make_tree(int x, int z, int h) -> void {

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

    make_tree(x, z, h);
}

void World::generate() {
    // Create a height map
    hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

    // Noise Map Settings
    NoiseUtil::NoiseSettings settings = {2,    1.0f, 2.0f,  0.42f,
                                         4.5f, 0.0f, 0.15f, 0.85f};

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
                    worldData[idx] = Block::Stone;
                else if (y >= (h - 4) && y < (h - 1))
                    worldData[idx] = Block::Dirt;
                else {
                    if (h < 32)
                        worldData[idx] = Block::Dirt;
                    else if (h == 32)
                        worldData[idx] = Block::Sand;
                    else
                        worldData[idx] = Block::Grass;
                }
            }

            if (h < 32) {
                for (int y = h; y < 32; y++) {
                    auto idx = (x * 256 * 64) + (z * 64) + y;
                    worldData[idx] = Block::Water;
                }
            }
        }
    }

    // Generate flowers
    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {

            srand((x | 1) << z * ~NoiseUtil::seed);
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

                worldData[idx] = Block::Flower1 + res % 2;

                continue;
            } else if (res < 6)
                continue;

            generate_tree(x, z, res);
        }
    }

    // Bottom of World = Bedrock
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            auto idx = (x * 256 * 64) + (z * 64) + 0;
            worldData[idx] = Block::Bedrock;
        }
    }

    // Update Lighting
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            update_lighting(x, z);
        }
    }

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

    // Draw flora
    for (auto const &[key, val] : chunks) {
        val->draw_flora();
    }

    // Draw transparent
    for (auto const &[key, val] : chunks) {
        val->draw_transparent();
    }

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
            blk == 40 || blk == 6) {
            auto idx2 = (x * 256 * 4) + (z * 4) + y / 16;
            lightData[idx2] |= 1 << (y % 16);
            continue;
        }

        auto idx2 = (x * 256 * 4) + (z * 4) + y / 16;
        lightData[idx2] |= 1 << (y % 16);
        break;
    }
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
