#include "CrossCraftGenerator.hpp"
#include "NoiseUtil.hpp"
#include "WorldGenUtil.hpp"
#include "../World.hpp"
extern "C" {
extern void generate(uint32_t seed, void *data);

extern double onoise(uint64_t octaves, double x, double y, uint32_t s);
extern double noise1(double x, double y);
extern double noise2(double x, double y);

void generate_heightmap(int32_t *heightMap) {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto xf = static_cast<float>(x);
            auto zf = static_cast<float>(z);

            auto heightLow = noise1(xf * 1.3, zf * 1.3) / 6.0 - 4.0;
            auto heightHigh = noise2(xf * 1.3, zf * 1.3) / 5.0 + 6.0;

            auto result = 0.0;

            if (onoise(6, xf, zf, 5) / 8.0 > 0) {
                result = heightLow;
            } else {
                if (heightHigh > heightLow)
                    result = heightHigh;
                else
                    result = heightLow;
            }

            result /= 2.0;

            if (result < 0)
                result *= 0.8;

            heightMap[x + z * 256] = 32 + result;
        }
    }
};

void smooth_heightmap(int32_t *heightMap) {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto xf = static_cast<float>(x);
            auto zf = static_cast<float>(z);

            auto a = noise1(xf * 2, zf * 2) / 8.0;
            int b = 0;

            if (noise2(xf * 2, zf * 2) > 0) {
                b = 1;
            }

            if (a > 2) {
                heightMap[x + z * 256] =
                    ((static_cast<float>(heightMap[x + z * 256]) - b) / 2.0) *
                        2 +
                    b;
            }
        }
    }
}
}
namespace CrossCraft {

auto create_surface(World *wrld, int32_t *heightMap) -> void {

    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto xf = static_cast<float>(x);
            auto zf = static_cast<float>(z);

            bool is_sand = onoise(8, xf, zf, 1) > 8.0;
            bool is_gravel = onoise(8, xf, zf, 2) > 12.0;

            auto y = heightMap[x + z * 256];

            auto blkA = wrld->worldData[wrld->getIdx(x, y + 1, z)];

            auto idx = wrld->getIdx(x, y, z);

            if (blkA == 8 && is_gravel) {
                wrld->worldData[idx] = 13;
            }

            if (blkA == 0) {
                if (y <= 32 && is_sand) {
                    wrld->worldData[idx] = 12;
                } else {
                    wrld->worldData[idx] = 2;
                }
            }
        }
    }
}

auto create_strata(World *wrld, int32_t *heightMap) -> void {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            auto dirtThickness =
                onoise(8, static_cast<float>(x), static_cast<float>(z), 1) /
                    24.0 -
                4.0;
            auto dirtTransition = heightMap[x + z * 256];
            auto stoneTransition = dirtTransition + dirtThickness;

            for (int y = 0; y < 64; y++) {
                u8 bType = 0;

                if (y == 0) {
                    bType = 10; // LAVA
                } else if (y <= stoneTransition) {
                    bType = 1; // STONE
                } else if (y <= dirtTransition) {
                    bType = 3; // DIRT
                }

                wrld->worldData[wrld->getIdx(x, y, z)] = bType;
            }
        }
    }
}

auto fill_water(World *wrld, int32_t *heightMap) -> void {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            int y = heightMap[x + z * 256];
            for (; y < 32; y++) {
                auto idx = wrld->getIdx(x, y, z);
                auto blk = wrld->worldData[idx];

                if (blk == 0) {
                    wrld->worldData[idx] = 8;
                }
            }
        }
    }
}

auto fill_lava(World *wrld, int32_t *heightMap) -> void {
    for (int z = 0; z < 256; z++) {
        for (int x = 0; x < 256; x++) {
            for (int y = 0; y < 10; y++) {
                auto idx = wrld->getIdx(x, y, z);
                auto blk = wrld->worldData[idx];

                if (blk == 0) {
                    wrld->worldData[idx] = 10;
                }
            }
        }
    }
}

auto create_flowers(World *wrld, int32_t *heightMap) -> void {
    auto flowerType = rand() % 2 + 37;
    auto px = rand() % 256;
    auto pz = rand() % 256;

    for (int i = 0; i < 10; i++) {
        auto fx = px;
        auto fz = pz;

        for (int c = 0; c < 5; c++) {
            fx += rand() % 6;
            fx -= rand() % 6;

            fz += rand() % 6;
            fz -= rand() % 6;

            if (fx >= 0 && fx < 256 && fz >= 0 && fz < 256) {
                auto fy = heightMap[fx + fz * 256] + 1;

                auto blk = wrld->worldData[wrld->getIdx(fx, fy, fz)];
                auto blkB = wrld->worldData[wrld->getIdx(fx, fy - 1, fz)];

                if (blk == 0 && blkB == 2) {
                    wrld->worldData[wrld->getIdx(fx, fy, fz)] = flowerType;
                }
            }
        }
    }
}

auto create_mushrooms(World *wrld, int32_t *heightMap) -> void {
    auto flowerType = rand() % 2 + 37;
    auto px = rand() % 256;
    auto py = rand() % 64;
    auto pz = rand() % 256;

    for (int i = 0; i < 10; i++) {
        auto fx = px;
        auto fy = py;
        auto fz = pz;

        for (int c = 0; c < 5; c++) {
            fx += rand() % 6;
            fx -= rand() % 6;

            fz += rand() % 6;
            fz -= rand() % 6;

            fy += rand() % 2;
            fy -= rand() % 2;

            if (fx >= 0 && fx < 256 && fz >= 0 && fz < 256 && fy >= 0 &&
                fy < 256) {

                auto blk = wrld->worldData[wrld->getIdx(fx, fy, fz)];
                auto blkB = wrld->worldData[wrld->getIdx(fx, fy - 1, fz)];

                if (blk == 0 && blkB == 2) {
                    wrld->worldData[wrld->getIdx(fx, fy, fz)] = flowerType;
                }
            }
        }
    }
}

auto isSpaceForTree(uint32_t x, uint32_t y, uint32_t z, World *wrld) -> bool {
    for (int i = 0; i < 6; i++) {
        int cx = x - 2;
        for (; cx <= x + 2; cx++) {
            int cz = z - 2;
            for (; cz <= z + 2; cz++) {
                auto idx = wrld->getIdx(cx, y + i, cz);
                auto blk = wrld->worldData[idx];

                if (blk != 0)
                    return false;
            }
        }
    }

    return true;
}

auto growTree(uint32_t x, uint32_t y, uint32_t z, uint32_t h, World *wrld)
    -> bool {
    if (x > 2 && z > 2 && y > 0 && y < 50 && x < 254 && z < 254) {
        if (isSpaceForTree(x, y, z, wrld)) {
            int max = y + h;

            uint32_t m = max;
            for (; m >= y; m--) {
                if (m == max) {
                    wrld->worldData[wrld->getIdx(x - 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z + 1)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z)] = 18;
                } else if (m == max - 1) {
                    wrld->worldData[wrld->getIdx(x - 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z + 1)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x - 1, m, z - 1)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x + 1, m, z - 1)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x - 1, m, z + 1)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x + 1, m, z + 1)] = 18;

                    wrld->worldData[wrld->getIdx(x, m, z)] = 17;

                } else if (m == max - 2 || m == max - 3) {
                    wrld->worldData[wrld->getIdx(x - 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z + 1)] = 18;
                    wrld->worldData[wrld->getIdx(x - 1, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x - 1, m, z + 1)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z + 1)] = 18;

                    wrld->worldData[wrld->getIdx(x - 2, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x - 2, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x - 2, m, z + 1)] = 18;

                    wrld->worldData[wrld->getIdx(x + 2, m, z - 1)] = 18;
                    wrld->worldData[wrld->getIdx(x + 2, m, z)] = 18;
                    wrld->worldData[wrld->getIdx(x + 2, m, z + 1)] = 18;

                    wrld->worldData[wrld->getIdx(x - 1, m, z + 2)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z + 2)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z + 2)] = 18;

                    wrld->worldData[wrld->getIdx(x - 1, m, z - 2)] = 18;
                    wrld->worldData[wrld->getIdx(x, m, z - 2)] = 18;
                    wrld->worldData[wrld->getIdx(x + 1, m, z - 2)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x - 2, m, z - 2)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x + 2, m, z - 2)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x - 2, m, z + 2)] = 18;

                    if (rand() % 2 == 0)
                        wrld->worldData[wrld->getIdx(x + 2, m, z + 2)] = 18;

                    wrld->worldData[wrld->getIdx(x, m, z)] = 17;

                } else {
                    wrld->worldData[wrld->getIdx(x, m, z)] = 17;
                }
            }
            return true;
        }
    }

    return false;
}

void create_tree(World *wrld, int32_t *heightMap) {
    auto px = rand() % 256;
    auto pz = rand() % 256;

    for (int i = 0; i < 20; i++) {
        auto tx = px;
        auto tz = pz;

        for (int c = 0; c < 5; c++) {
            tx += rand() % 6;
            tx -= rand() % 6;

            tz += rand() % 6;
            tz -= rand() % 6;

            if (tx >= 0 && tx < 256 && tz >= 0 && tz < 256 &&
                static_cast<float>(rand() % 128) / 128.0f <= 0.25) {
                auto ty = heightMap[tx + tz * 256] + 1;

                auto th = 4 + rand() % 3;

                growTree(tx, ty, tz, th, wrld);
            }
        }
    }
}

void create_plants(World *wrld, int32_t *heightMap) {
    const int numFlowers = 256 * 256 / 3000;
    const int numMushrooms = 256 * 256 * 64 / 2000;
    const int numTrees = 256 * 256 / 4000;

    for (int i = 0; i < numFlowers; i++) {
        create_flowers(wrld, heightMap);
    }
    for (int i = 0; i < numMushrooms; i++) {
        create_mushrooms(wrld, heightMap);
    }
    for (int i = 0; i < numTrees; i++) {
        create_tree(wrld, heightMap);
    }
}

auto CrossCraftGenerator::generate(World *wrld) -> void {

    int32_t *heightMap = (int32_t *)calloc(256 * 256, sizeof(int32_t));

    //"Raising..."
    generate_heightmap(heightMap);
    //"Eroding..."
    smooth_heightmap(heightMap);

    srand(NoiseUtil::seed);

    //"Soiling..."
    create_strata(wrld, heightMap);

    // Ores
    // make_ores();

    //"Watering..."
    fill_water(wrld, heightMap);

    //"Melting..."
    fill_lava(wrld, heightMap);

    //"Growing..."
    create_surface(wrld, heightMap);

    ////"Planting..."
    create_plants(wrld, heightMap);

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