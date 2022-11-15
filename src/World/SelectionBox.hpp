#pragma once
#include "World.hpp"

namespace CrossCraft {
class World;
class SelectionBox {
  public:
    SelectionBox();

    auto update_position(World *wrld) -> void;
    auto draw() -> void;

  private:
    glm::ivec3 position;

    uint16_t idx_counter;
    Rendering::Mesh<Rendering::Vertex> blockMesh;
};
} // namespace CrossCraft