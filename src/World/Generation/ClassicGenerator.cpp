#include "ClassicGenerator.hpp"
#include "NoiseUtil.hpp"
#include "WorldGenUtil.hpp"

namespace CrossCraft {

auto ClassicGenerator::generate_tree(World *wrld, int x, int z) -> void {

    if (!(x >= 0 && x < 256 && z >= 0 && z < 256))
        return;

    int h = wrld->hmap[x * 256 + z] * 64.f;

    if (h < 33)
        return;

    WorldGenUtil::make_tree(wrld, x, z, h);
}

auto ClassicGenerator::generate(World *wrld) -> void {
    // Create a height map
    wrld->hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

    // Noise Map Settings
    NoiseUtil::NoiseSettings settings = {2,    1.0f, 2.0f,  0.42f,
                                         4.5f, 0.0f, 0.15f, 0.85f};
    NoiseUtil::NoiseSettings settings2 = {1,    1.0f, 32.0f, 0.42f,
                                          4.5f, 0.0f, 0.0f,  1.0f};

    // Generate HMAP
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            wrld->hmap[x * 256 + z] = get_noise(x, z, &settings);
        }
    }

    // Generate world data / fill data
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            int h = wrld->hmap[x * 256 + z] * 64.f;
            for (int y = 0; y < h; y++) {
                auto idx = (x * 256 * 64) + (z * 64) + y;
                if (y == 1)
                    wrld->worldData[idx] = Block::Lava;
                else if (y < (h - 4))
                    wrld->worldData[idx] = Block::Stone;
                else if (y >= (h - 4) && y < (h - 1))
                    wrld->worldData[idx] = Block::Dirt;
                else {
                    if (h < 32)
                        wrld->worldData[idx] = Block::Dirt;
                    else if (h == 32)
                        wrld->worldData[idx] = Block::Sand;
                    else
                        wrld->worldData[idx] = Block::Grass;
                }
            }

            if (h < 32) {
                for (int y = h; y < 32; y++) {
                    auto idx = (x * 256 * 64) + (z * 64) + y;
                    wrld->worldData[idx] = Block::Water;
                }
            }
        }
    }

    srand(NoiseUtil::seed);

    // Generate flowers

    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            int h = wrld->hmap[x * 256 + z] * 64.f;
            auto idx = (x * 256 * 64) + (z * 64) + h;

            auto v = get_noise(x, z, &settings2);

            if (v > 0.8f && h >= 33) {
                int r = rand() % 5;
                if (r < 2)
                    wrld->worldData[idx] = Block::Flower1 + r;
            }
        }
    }

    // Generate trees
    for (int i = 0; i < 384; i++) {
        uint8_t x = rand() % 256;
        uint8_t z = rand() % 256;

        generate_tree(wrld, x, z);
    }

    // Bottom of World = Bedrock
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            auto idx = (x * 256 * 64) + (z * 64) + 0;
            wrld->worldData[idx] = Block::Bedrock;
        }
    }

    // Update Lighting
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            wrld->update_lighting(x, z);
        }
    }
}

} // namespace CrossCraft