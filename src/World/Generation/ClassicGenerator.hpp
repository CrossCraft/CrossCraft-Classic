#pragma once
#include "../World.hpp"

namespace CrossCraft {

class ClassicGenerator {
  public:
    static auto generate(World *wrld) -> void;

  private:
    static auto generate_tree(World *wrld, int x, int z, int hash) -> void;
};
} // namespace CrossCraft