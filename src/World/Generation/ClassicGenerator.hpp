#pragma once
#include "../World.hpp"

namespace CrossCraft {

class ClassicGenerator {
  public:
    static auto generate(World *wrld) -> void;

  private:
    static auto generate_tree(World *wrld, int x, int z) -> void;
    static auto setBlk(int x, int y, int z, uint8_t blk, uint8_t *data) -> void;
};
} // namespace CrossCraft