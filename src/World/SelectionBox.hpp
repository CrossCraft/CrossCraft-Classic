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
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh blockMesh;
};
} // namespace CrossCraft