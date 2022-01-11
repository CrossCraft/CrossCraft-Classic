#pragma once
#include "ChunkStack.hpp"
#include "Player.hpp"
#include <FastNoiseLite.h>
#include <Utilities/Types.hpp>
#include <map>
#include <memory>
#include <stdint.h>

namespace CrossCraft {

typedef uint8_t block_t;
class ChunkStack;

struct NoiseSettings {
    uint8_t octaves;
    float amplitude;
    float frequency;
    float persistence;
    float mod_freq;
    float offset;

    float range_min;
    float range_max;
};

class World {
  public:
    World(RefPtr<Player> p);
    ~World();

    auto get_noise(float x, float y, NoiseSettings *settings) -> float;

    auto update(double dt) -> void;
    auto draw() -> void;

    auto generate() -> void;

    auto getBlock(int x, int y, int z) -> block_t;
    block_t *worldData;

  private:
    ChunkStack *stack;
    RefPtr<Player> player;
    glm::ivec2 lastPlayerPos;
    unsigned int terrain_atlas;
    FastNoiseLite fsl;
};

} // namespace CrossCraft