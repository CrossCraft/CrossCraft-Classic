#include "Player.hpp"
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(0.f, 32.8f, -1.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.2f, 255.0f) {}

void Player::update(float dt) {

    float rotSpeed = 50.0f;

    using namespace Utilities::Input;
    float cX, cY;
    get_cursor_pos(cX, cY);

    cX = (cX - 0.5f) * 2;
    cY = (cY - 0.5f) * 2;

    // rot.x += cX * rotSpeed * dt;
    // rot.y += cY * rotSpeed * dt;

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

    dt /= 50.0f;
    float playerSpeed = 4.3f;

    vel = glm::vec3(0.f, 0.f, 0.f);

    if (get_action_state(Action::Forward)) {
        vel.x += sinf(DEGTORAD(-rot.y)) * playerSpeed * dt;
        vel.z += cosf(DEGTORAD(-rot.y)) * playerSpeed * dt;
    }

    if (get_action_state(Action::Backward)) {
        vel.x += -sinf(DEGTORAD(-rot.y)) * playerSpeed * dt;
        vel.z += -cosf(DEGTORAD(-rot.y)) * playerSpeed * dt;
    }

    if (get_action_state(Action::Left)) {
        vel.x += sinf(DEGTORAD(-rot.y + 90.f)) * playerSpeed * dt;
        vel.z += cosf(DEGTORAD(-rot.y + 90.f)) * playerSpeed * dt;
    }

    if (get_action_state(Action::Right)) {
        vel.x += -sinf(DEGTORAD(-rot.y + 90.f)) * playerSpeed * dt;
        vel.z += -cosf(DEGTORAD(-rot.y + 90.f)) * playerSpeed * dt;
    }

    pos += vel;

    // Camera
    cam.pos = {pos.x, -pos.y, pos.z};
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
}

} // namespace CrossCraft