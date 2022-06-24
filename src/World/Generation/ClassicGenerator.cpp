#include "ClassicGenerator.hpp"
#include "NoiseUtil.hpp"
#include "WorldGenUtil.hpp"

namespace CrossCraft {

auto ClassicGenerator::generate_tree(World *wrld, int x, int z, int hash)
    -> void {
    x *= 8;
    z *= 8;

    x += rand() % 5 - 2;
    z += rand() % 5 - 2;

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

                int h = wrld->hmap[xf * 256 + zf] * 64.f;
                if ((h + 1) <= 33)
                    continue;

                auto idx = (xf * 256 * 64) + (zf * 64) + h;

                wrld->worldData[idx] = Block::Flower1 + res % 2;

                continue;
            } else if (res < 6)
                continue;

            generate_tree(wrld, x, z, res);
        }
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