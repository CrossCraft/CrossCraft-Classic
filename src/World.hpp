#pragma once
#include "Player.hpp"
#include <Utilities/Types.hpp>
#include <map>
#include <memory>
#include <stdint.h>

namespace CrossCraft {

typedef uint8_t block_t;

class World {
public:
  World(RefPtr<Player> p);
  ~World();

  auto update(double dt) -> void;
  auto draw() -> void;

  auto generate() -> void;

  auto getBlock(int x, int y, int z) -> block_t;

private:
  RefPtr<Player> player;
  glm::ivec2 lastPlayerPos;
  unsigned int terrain_atlas;
  block_t worldData[256 * 64 * 256];
};

} // namespace CrossCraft