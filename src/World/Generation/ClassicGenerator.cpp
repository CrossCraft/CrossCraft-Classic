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

auto ClassicGenerator::setBlk(int x, int y, int z, uint8_t blk, uint8_t *data)
    -> void {
    auto idx = (y * 256 * 256) + (z * 256) + x;

    if (idx >= 0 && idx < (256 * 64 * 256) && data[idx] == Block::Stone) {
        data[idx] = blk;
    }
}

auto ClassicGenerator::generate(World *wrld) -> void {
    // Create a height map
    wrld->hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

    // Noise Map Settings
    NoiseUtil::NoiseSettings settings = {1,    1.0f, 16.0f, 0.8f,
                                         4.0f, 0.0f, 0.4f,  0.7f};
    NoiseUtil::NoiseSettings settings2 = {1,    1.0f, 32.0f, 0.42f,
                                          4.5f, 0.0f, 0.0f,  1.0f};

    // Generate HMAP
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            wrld->hmap[x * 256 + z] = get_noise(x, z, &settings) +
                                      0.025f; // * get_noise(x, z, &settings3);
        }
    }

    // Generate world data / fill data
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            int h = wrld->hmap[x * 256 + z] * 64.f;
            for (int y = 0; y < h; y++) {
                auto idx = (y * 256 * 256) + (z * 256) + x;
                if (y == 1)
                    wrld->worldData[idx] = Block::Lava;
                else if (y < (h - 4))
                    wrld->worldData[idx] = Block::Stone;
                else if (y >= (h - 4) && y < (h - 1))
                    wrld->worldData[idx] = Block::Dirt;
                else {
                    if (h < 32) {
                        auto v = get_noise(x, z, &settings2);
                        if (v > 0.4f)
                            wrld->worldData[idx] = Block::Dirt;
                        else
                            wrld->worldData[idx] = Block::Gravel;
                    } else if (h == 32)
                        wrld->worldData[idx] = Block::Sand;
                    else
                        wrld->worldData[idx] = Block::Grass;
                }
            }

            if (h < 32) {
                for (int y = h; y < 32; y++) {
                    auto idx = (y * 256 * 256) + (z * 256) + x;
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
            auto idx = (h * 256 * 256) + (z * 256) + x;

            auto v = get_noise(x, z, &settings2);

            if (v > 0.8f && h >= 33) {
                int r = rand() % 5;
                if (r < 2)
                    wrld->worldData[idx] = Block::Flower1 + r;
            }
        }
    }

    // Generate trees
    for (int i = 0; i < 256; i++) {
        uint8_t x = rand() % 256;
        uint8_t z = rand() % 256;

        generate_tree(wrld, x, z);
    }

    // Generate Coal
    for (int c = 0; c < 256 * 64; c++) {
        uint8_t x = rand() % 256;
        uint8_t z = rand() % 256;
        uint8_t y = rand() % 20 + 10;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    setBlk(x + i, y + j, z + k, Block::Coal_Ore,
                           wrld->worldData);
    }
    // Generate Iron
    for (int c = 0; c < 256 * 32; c++) {
        uint8_t x = rand() % 256;
        uint8_t z = rand() % 256;
        uint8_t y = rand() % 15 + 7;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    setBlk(x + i, y + j, z + k, Block::Iron_Ore,
                           wrld->worldData);
    }
    // Generate Gold
    for (int c = 0; c < 256 * 20; c++) {
        uint8_t x = rand() % 256;
        uint8_t z = rand() % 256;
        uint8_t y = rand() % 10 + 5;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    setBlk(x + i, y + j, z + k, Block::Gold_Ore,
                           wrld->worldData);
    }

    // Bottom of World = Bedrock
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            auto idx = (0 * 256 * 256) + (z * 256) + x;
            wrld->worldData[idx] = Block::Bedrock;
        }
    }

    // Update Lighting
    for (int x = 0; x < 256; x++) {
        for (int z = 0; z < 256; z++) {
            wrld->update_lighting(x, z);
        }
    }

    wrld->generate_meta();
}

} // namespace CrossCraft