/**
 * @file Player.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Player controller
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../World/World.hpp"
#include "AABB.hpp"
#include "Graphics/2D/FontRenderer.hpp"
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include "../UI/TextHelper.hpp"
#include <any>
#include <glm.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class World;


/**
 * @brief Player controller object
 *
 */
class Player {
  public:
    /**
     * @brief Construct a new Player object
     *
     */
    Player();

    /**
     * @brief Destroy the Player object
     *
     */
    ~Player() = default;

    /**
     * @brief Updates the player position and camera object
     *
     * @param dt Delta Time
     */
    auto update(float dt, World *wrld) -> void;

    /**
     * @brief Draw the player UI
     *
     */
    auto draw() -> void;

    auto spawn(World *wrld) -> void;

    /**
     * @brief Get the position
     *
     * @return glm::vec3
     */
    inline auto get_pos() -> glm::vec3 { return pos; }

    /**
     * @brief Get the rotation
     *
     * @return glm::vec3
     */
    inline auto get_rot() -> glm::vec2 { return rot; }

    static auto move_forward(std::any p) -> void;
    static auto move_backward(std::any p) -> void;
    static auto move_left(std::any p) -> void;
    static auto move_right(std::any p) -> void;

    static auto move_up(std::any p) -> void;
    static auto move_down(std::any p) -> void;
    static auto move_reset(std::any p) -> void;

    static auto respawn(std::any p) -> void;

    static auto change_selector(std::any p) -> void;
    static auto inc_selector(std::any p) -> void;
    static auto dec_selector(std::any p) -> void;

    static auto toggle_inv(std::any p) -> void;

    int32_t selectorIDX;
    uint8_t itemSelections[9];

    uint32_t terrain_atlas;
    bool in_inventory;

    uint8_t inventorySelection[45];

    float vcursor_x, vcursor_y;

  private:
    auto rotate(float dt, float sense) -> void;
    auto test_collide(glm::vec3 pos, World *wrld, float dt) -> void;

    auto setup_model(uint8_t type) -> void;

    auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          uint32_t lightVal, glm::vec3 pos, uint8_t type)
        -> void;

    auto drawBlk(uint8_t type, int x, int y) -> void;
    auto drawBlkHand(uint8_t type) -> void;

    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;

    float jump_icd;
    float view_bob, view_timer;
    float cube_bob;

    Rendering::Camera cam;
    ScopePtr<Graphics::G2D::Sprite> item_box;
    ScopePtr<Graphics::G2D::Sprite> selector;
    ScopePtr<Graphics::G2D::Sprite> crosshair;
    ScopePtr<Graphics::G2D::Sprite> water;
    ScopePtr<Graphics::G2D::Sprite> overlay;
    uint32_t gui_texture, water_texture, overlay_texture, font_texture;

    AABB model;

    bool is_falling, is_underwater, is_head_water, water_cutoff;
    bool on_ground, jumping;

    // Block Drawing

    uint16_t idx_counter[50];
    std::vector<Rendering::Vertex> m_verts[50];
    std::vector<uint16_t> m_index[50];
    Rendering::Mesh blockMesh[50];

    ScopePtr<TextHelper> textHelper;
};

struct SelData {
    Player *player;
    int selIDX;
};

struct RespawnRequest {
    Player* player;
    World* wrld;
};

} // namespace CrossCraft
