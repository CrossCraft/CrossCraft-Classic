#pragma once
#include "../MP/Client.hpp"
#include "../UI/TextHelper.hpp"
#include "../UI/UserInterface.hpp"
#include "../World/World.hpp"
#include "AABB.hpp"
#include "Chat.hpp"
#include "Graphics/2D/FontRenderer.hpp"
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
using namespace Stardust_Celeste;

namespace CrossCraft {
class BlockRep {
  public:
    BlockRep();
    ~BlockRep();

    auto drawBlk(uint8_t type, int x, int y, int y_offset, float scale) -> void;
    auto drawBlkHand(uint8_t type, World *wrld, double cube_bob) -> void;

    uint32_t terrain_atlas;

  private:
    // Block Drawing

    auto setup_model(uint8_t type) -> void;

    auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          uint32_t lightVal, glm::vec3 pos, uint8_t type)
        -> void;

    uint16_t idx_counter[50];
    std::vector<Rendering::Vertex> m_verts[50];
    std::vector<uint16_t> m_index[50];
    Rendering::Mesh blockMesh[50];
};
} // namespace CrossCraft
