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
#include "AABB.hpp"
#include "World.hpp"
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
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

    static auto change_selector(std::any p) -> void;
    static auto inc_selector(std::any p) -> void;
    static auto dec_selector(std::any p) -> void;

    int32_t selectorIDX;
    uint8_t itemSelections[9];

  private:
    auto rotate(float dt) -> void;
    auto test_collide(glm::vec3 pos, World *wrld) -> void;

    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;

    Rendering::Camera cam;
    ScopePtr<Graphics::G2D::Sprite> item_box;
    ScopePtr<Graphics::G2D::Sprite> selector;
    ScopePtr<Graphics::G2D::Sprite> crosshair;
    ScopePtr<Graphics::G2D::Sprite> water;
    uint32_t gui_texture, water_texture;

    AABB model;

    bool is_falling, is_underwater, is_head_water;
};

struct SelData {
    Player *player;
    int selIDX;
};

} // namespace CrossCraft