#pragma once
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

    inline auto get_pos() -> glm::vec3 { return pos; }

  private:
    glm::vec3 pos;
    glm::vec2 rot;
    glm::vec3 vel;

    Rendering::Camera cam;
};
} // namespace CrossCraft