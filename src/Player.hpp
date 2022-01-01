#pragma once
#include "AABB.h"
#include <Rendering/Camera.hpp>
#include <glm.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class Player {
  public:
    Player();
    ~Player() = default;

    auto update(float dt) -> void;
    auto draw() -> void{};

  private:
    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;

    Rendering::Camera cam;
};
} // namespace CrossCraft