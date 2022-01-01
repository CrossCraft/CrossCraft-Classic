#include "Player.hpp"
#include <Utilities/Input.hpp>
#include <gtx/projection.hpp>

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(0.f, 40.f, 0.f), rot(0.f, 0.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.2f, 255.0f) {}

void Player::update(float dt) {

    float rotSpeed = 50.0f;

    // TODO: Rotate cam

    if (rot.y > 360.0f) {
        rot.y -= 360.0f;
    }

    if (rot.y < 0.0f) {
        rot.y += 360.0f;
    }

    if (rot.x < -89.9f) {
        rot.x = -89.9f;
    }

    if (rot.x > 89.9f) {
        rot.x = 89.9f;
    }

    float playerSpeed = 4.3f;

    // TODO: Change Player Velocity

    pos += vel * dt;

    // Camera
    cam.pos = pos;
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
}

} // namespace CrossCraft