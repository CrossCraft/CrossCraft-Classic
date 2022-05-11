#include "Player.hpp"
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>
namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(8.f, 40.8f, 8.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.2f, 255.0f),
      blkSel(1), is_falling(true) {}

const auto playerSpeed = 4.3f;

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->vel.x += -sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
    p->vel.z += -cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->vel.x += sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
    p->vel.z += cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
    p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
}

auto Player::move_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
    p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
}

auto Player::move_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->is_falling) {
        p->vel.y = 8.4f;
    }
}

auto Player::move_down(std::any d) -> void {
    // TODO: Sneak
}

auto Player::rotate(float dt) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 600.0f;
    float cX, cY;

#ifndef PSP
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");
#else
    cX = get_axis("PSP", "X");
    cY = get_axis("PSP", "Y");

    if (cX <= 0.25f && cX >= -0.25f)
        cX = 0.0f;
    if (cY <= 0.25f && cY >= -0.25f)
        cY = 0.0f;
#endif

    rot.y += cX * rotSpeed * dt;
    rot.x += cY * rotSpeed * dt;

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

    set_cursor_center();
}

const float GRAVITY_ACCELERATION = 28.0f;

void Player::update(float dt, World *wrld) {
    rotate(dt);

    // Update position
    vel.y -= GRAVITY_ACCELERATION * dt;
    is_falling = true;
    glm::vec3 testpos = pos + vel * dt;
    auto idx = wrld->getIdx(testpos.x, testpos.y - 1.8f, testpos.z);

    if (wrld->worldData[idx] != 0) {
        vel.y = 0;
        is_falling = false;
    }

    pos += vel * dt;

    // When the player stops falling, we make sure the player snaps to the top
    // of a surface
    if (!is_falling) {
        pos.y += 0.2f;
        pos.y = std::round(pos.y);
        pos.y -= 0.2f;
    }

    // Update camera
    cam.pos = pos;
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
    vel = glm::vec3(0.f, vel.y, 0.f);
}

} // namespace CrossCraft