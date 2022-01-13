#include "World.hpp"
#include <Rendering/Rendering.hpp>
#include <iostream>

#if PSP
#include <pspkernel.h>
#endif

namespace CrossCraft {
World::World(std::shared_ptr<Player> p) {
    player = p;
    pchunk_pos = {-1, -1};

    terrain_atlas = Rendering::TextureManager::get().load_texture(
        "./assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true);
    fsl.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    fsl.SetFrequency(0.001f * 5.f);
    fsl.SetSeed(time(NULL));

    // Zero the array
    worldData =
        reinterpret_cast<block_t *>(calloc(256 * 64 * 256, sizeof(block_t)));

    chunks.clear();
}

World::~World() {
    Rendering::TextureManager::get().delete_texture(terrain_atlas);
    free(worldData);

    for (auto const &[key, val] : chunks) {
        if (val)
            delete val;
    }
    chunks.clear();
}

const auto RENDER_DISTANCE_DIAMETER = 6.f;

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
    player->update(static_cast<float>(dt));

    // TODO: Update world meshes
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

    for (auto i = 0; i < settings->octaves; i++) {
        auto noise = fsl.GetNoise(x * freq, y * freq);

        noise *= amp;
        sum_noise += noise;
        sum_amp += amp;

        amp *= settings->persistence;
        freq *= settings->mod_freq;
    }

    auto divided = sum_noise / sum_amp;
    range_map(divided, -1.0f, 1.0f, settings->range_min, settings->range_max);

    return divided;
}

void World::generate() {

    float *hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

    NoiseSettings settings = {2, 1.0f, 2.0f, 0.42f, 4.5f, 0.0f, 0.15f, 0.85f};

    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            hmap[x * 256 + z] = get_noise(x, z, &settings);
        }
    }

    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            int h = hmap[x * 256 + z] * 64.f;
            for (int y = 0; y < h; y++) {
                worldData[(x * 256 * 64) + (z * 64) + y] = 1;
            }
        }
    }

    free(hmap);
}

void World::draw() {

    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    for (auto const &[key, val] : chunks) {
        val->draw();
    }

    for (auto const &[key, val] : chunks) {
        val->draw_transparent();
    }

    player->draw();
}

block_t World::getBlock(int x, int y, int z) {
    int idx = ((y * 128) + z) * 128 + x;
    return worldData[idx];
}

} // namespace CrossCraft
