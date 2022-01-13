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
#include <Rendering/Camera.hpp>
#include <glm.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

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
    auto update(float dt) -> void;

    /**
     * @brief Draw the player UI
     *
     */
    auto draw() -> void{};

    /**
     * @brief Get the position
     *
     * @return glm::vec3
     */
    inline auto get_pos() -> glm::vec3 { return pos; }

  private:
    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;

    Rendering::Camera cam;
};
} // namespace CrossCraft