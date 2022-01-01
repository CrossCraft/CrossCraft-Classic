#pragma once
#include "AABB.h"
#include "World.h"
#include <Rendering/Camera.hpp>
#include <glm.hpp>

using namespace Stardust;

class World;
/**
 * Describes the player.
 */
class Player {
  public:
    Player();
    ~Player();

    /**
     * Updates position.
     *
     * \param dt - based on delta time
     */
    void update(double dt, World *wrld);

    /**
     * Draws the GUI.
     */
    void draw();

    glm::vec3 pos;
    glm::vec2 rot;

  private:
    glm::vec3 vel;
};