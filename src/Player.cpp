#include "Player.hpp"
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>
namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(8.f, 40.8f, 8.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), 70.0f, 16.0f / 9.0f, 0.01f, 255.0f),
      is_falling(true),
      model({8, 40.8, 8}, {0.4, 1.8, 0.4}), itemSelections{1,  4,  45, 2, 5,
                                                           17, 18, 20, 44} {
    gui_texture = Rendering::TextureManager::get().load_texture(
        "./assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    water_texture = Rendering::TextureManager::get().load_texture(
        "./assets/water.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{149, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{148, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});
    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{240 - 8, 136 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {480, 272}});

    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, 30, -30);

    selectorIDX = 0;
    is_underwater = false;
}

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
        p->is_falling = false;
    }

    if (p->is_underwater) {
        p->vel.y = 3.2f;
        p->is_falling = false;
    }
}

auto Player::change_selector(std::any d) -> void {
    auto s = std::any_cast<SelData>(d);
    s.player->selectorIDX = s.selIDX;
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX += 1;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX -= 1;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;
}

auto Player::move_down(std::any d) -> void {
    // TODO: Sneak
}

auto Player::rotate(float dt) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 500.0f;
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

auto test(glm::vec3 pos, World *wrld) -> bool {
    auto blk = wrld->worldData[wrld->getIdx(pos.x, pos.y, pos.z)];
    return blk != 0 && blk != 8 && blk != 6 && blk != 37 && blk != 38 &&
           blk != 39 && blk != 40;
}

void Player::test_collide(glm::vec3 testpos, World *wrld) {

    bool collide_down = false;
    collide_down =
        collide_down || test({testpos.x, testpos.y - 1.8f, testpos.z}, wrld);

    if (collide_down) {
        vel.y = 0;
        is_falling = false;
    }

    for (int x = -1; x <= 1; x++)
        for (int y = 0; y <= 2; y++)
            for (int z = -1; z <= 1; z++) {
                float xoff = x;
                float zoff = z;

                auto new_vec = glm::vec3(testpos.x + xoff, testpos.y - 1.8f + y,
                                         testpos.z + zoff);

                if (test(new_vec, wrld)) {
                    AABB test_box = AABB(new_vec, {1, 1, 1});

                    if (AABB::intersect(test_box, model)) {
                        vel.x = 0;
                        vel.z = 0;
                    }
                }
            }

    if (test({testpos.x, testpos.y, testpos.z}, wrld)) {
        vel.y = 0;
        is_falling = true;
    }
}

void Player::update(float dt, World *wrld) {
    rotate(dt);

    // Update position
    if (!is_underwater)
        vel.y -= GRAVITY_ACCELERATION * dt;
    else
        vel.y -= GRAVITY_ACCELERATION * 0.3f * dt;
    is_falling = true;

    glm::vec3 testpos = pos + vel * dt;
    model.pos = testpos + glm::vec3(0.2f, 0, 0.2f);

    auto blk =
        wrld->worldData[wrld->getIdx(testpos.x, testpos.y + 0.2f, testpos.z)];
    if (blk == 8)
        is_head_water = true;
    else
        is_head_water = false;

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.5f, testpos.z)];
    if (blk == 8)
        is_underwater = true;
    else
        is_underwater = false;

    test_collide(testpos, wrld);

    if (is_underwater) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.9f;
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

auto Player::draw() -> void {
    selector->set_position({148 + 20 * selectorIDX, 0});
    selector->draw();

    item_box->set_position({149, 1});
    item_box->draw();

    crosshair->set_position({240 - 8, 136 - 8});
    crosshair->draw();

    if (is_head_water) {
        water->set_position({0, 0});
        water->draw();
    }

    Rendering::RenderContext::get().set_mode_3D();
    Rendering::RenderContext::get().matrix_perspective(60.0f, 16.0f / 9.0f,
                                                       0.1f, 255.0f);
}
} // namespace CrossCraft