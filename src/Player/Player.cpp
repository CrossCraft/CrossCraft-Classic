#include "Player.hpp"
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>

#if PSP
#include <pspctrl.h>
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

Player::Player()
    : pos(0.f, 64.0f, 0.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), DEGTORAD(70.0f), 16.0f / 9.0f, 0.1f,
          255.0f),
      is_falling(true),
      model(pos, {0.6, 1.8, 0.6}), itemSelections{1,  4,  45, 2, 5,
                                                  17, 18, 20, 44},
      inventorySelection{1,  4,  45, 2,  5,  17, 18, 20, 44, 48, 6,
                         37, 38, 39, 40, 12, 13, 19, 21, 22, 23, 24,
                         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                         36, 14, 15, 16, 42, 41, 47, 46, 49} {
    gui_texture = Rendering::TextureManager::get().load_texture(
        "./assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    water_texture = Rendering::TextureManager::get().load_texture(
        "./assets/water.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    overlay_texture = Rendering::TextureManager::get().load_texture(
        "./assets/overlay.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    font_texture = Rendering::TextureManager::get().load_texture(
        "./assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

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
    overlay = create_scopeptr<Graphics::G2D::Sprite>(
        overlay_texture, Rendering::Rectangle{{120 - 8, 64 - 44}, {256, 256}});

    textHelper = create_scopeptr<TextHelper>();

    selectorIDX = 0;
    is_underwater = false;
    in_inventory = false;
    jump_icd = 0.2f;
    terrain_atlas = 0;
    view_timer = 0.0f;

    for (int i = 0; i < 50; i++) {
        setup_model(i);
    }

    on_ground = false;
    jumping = false;

    vcursor_x = 240;
    vcursor_y = 136;
}

const auto playerSpeed = 4.3f;

auto Player::spawn(World *wrld) -> void {
    bool spawned = false;

    while (!spawned) {
        int x = rand() % 64 - 32 + 128;
        int z = rand() % 64 - 32 + 128;

        for (int y = 50; y > 32; y--) {
            auto blk = wrld->worldData[wrld->getIdx(x, y, z)];

            if (blk != 0 && blk != 8) {
                pos = {x + 0.5f, y + 1, z + 0.5f};
                pos.y += 1.8f;

                cam.pos = pos;
                model.pos = pos;

                SC_APP_INFO("SPAWNED AT {} {} {} {}", x, y, z, blk);
                return;
            }
        }
    }
}

auto Player::add_face_to_mesh(std::array<float, 12> data,
                              std::array<float, 8> uv, uint32_t lightVal,
                              glm::vec3 mypos, uint8_t type)
    -> void { // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {
        m_verts[type].push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            data[idx++] + mypos.x,
            data[idx++] + mypos.y,
            data[idx++] + mypos.z,
        });
    }

    // Push Back Indices
    m_index[type].push_back(idx_counter[type]);
    m_index[type].push_back(idx_counter[type] + 1);
    m_index[type].push_back(idx_counter[type] + 2);
    m_index[type].push_back(idx_counter[type] + 2);
    m_index[type].push_back(idx_counter[type] + 3);
    m_index[type].push_back(idx_counter[type] + 0);
    idx_counter[type] += 4;
}

auto Player::setup_model(uint8_t type) -> void {
    idx_counter[type] = 0;
    m_verts[type].clear();
    m_verts[type].shrink_to_fit();
    m_index[type].clear();
    m_index[type].shrink_to_fit();
    blockMesh[type].delete_data();

    glm::vec3 p = {0, 0, 0};

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40) {

        add_face_to_mesh(xFace1, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                         p, type);
        add_face_to_mesh(xFace2, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                         p, type);
    } else if (type == 44) {
        add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP,
                         {0, -0.5, 0}, type);
        add_face_to_mesh(leftFaceHalf, getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_BOT, p, type);
        add_face_to_mesh(backFaceHalf, getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_SIDE_X, {0, 0, 1}, type);
        add_face_to_mesh(frontFaceHalf, getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, {0, 0, 1}, type);
        add_face_to_mesh(backFaceHalf, getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, p, type);
    } else {

        add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP, p,
                         type);
        add_face_to_mesh(leftFace, getTexCoord(type, LIGHT_SIDE_X), LIGHT_BOT,
                         p, type);
        add_face_to_mesh(backFace, getTexCoord(type, LIGHT_SIDE_X),
                         LIGHT_SIDE_X, {0, 0, 1}, type);
        add_face_to_mesh(frontFace, getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, {0, 0, 1}, type);
        add_face_to_mesh(backFace, getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, p, type);
    }

    blockMesh[type].add_data(m_verts[type].data(), m_verts[type].size(),
                             m_index[type].data(), idx_counter[type]);
}

auto Player::move_reset(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    p->vel.x = 0.0f;
    p->vel.z = 0.0f;
}

bool hasDir = false;

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling)) {

        if (!hasDir) {
            p->vel.x = -sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
            p->vel.z = -cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
            hasDir = true;
        }
    }
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling)) {

        if (!hasDir) {
            p->vel.x = sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
            p->vel.z = cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
            hasDir = true;
        }
    }
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling)) {

        if (!hasDir) {
            p->vel.x = -sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
            p->vel.z = -cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        } else {
            p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
            p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        }
    }
}

auto Player::respawn(std::any d) -> void {
    auto p = std::any_cast<RespawnRequest>(d);
    p.player->spawn(p.wrld);
}

auto Player::move_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling)) {

        if (!hasDir) {
            p->vel.x = sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
            p->vel.z = cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        } else {
            p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
            p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f)) * playerSpeed;
        }
    }
}

auto Player::move_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory) {
        if (!p->jumping && p->jump_icd < 0.0f && !p->is_underwater &&
            !p->is_falling) {
            p->vel.y = 8.4f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = false;
        }

        if (p->is_underwater && p->water_cutoff) {
            p->vel.y = 3.2f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = true;
        }
    }
}

auto Player::change_selector(std::any d) -> void {
    auto s = std::any_cast<SelData>(d);
    s.player->selectorIDX = s.selIDX;
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX++;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX--;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;
}

auto Player::toggle_inv(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->in_inventory = !p->in_inventory;

#if BUILD_PC
    if (p->in_inventory)
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Utilities::Input::set_cursor_center();
#else
    p->vcursor_x = 240;
    p->vcursor_y = 136;
#endif
}

auto Player::move_down(std::any d) -> void {
    // TODO: Sneak
}

auto Player::rotate(float dt, float sense) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 500.0f;
    float cX, cY;

#ifndef PSP
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");

    cX *= 3.0f;
    cY *= 3.0f;
#else
    cX = get_axis("PSP", "X");
    cY = get_axis("PSP", "Y");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;
#endif
    if (!in_inventory) {
        rot.y += cX * rotSpeed * dt * sense;
        rot.x += cY * rotSpeed * dt * sense;

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
    } else {
        vcursor_x += cX * 20.0f * 2.5f * dt / 0.3f * sense;
        vcursor_y += -cY * 20.0f * 2.5f * dt / 0.3f * sense;

        if (vcursor_x < 0)
            vcursor_x = 0;
        else if (vcursor_x > 480)
            vcursor_x = 480;
        if (vcursor_y < 0)
            vcursor_y = 0;
        else if (vcursor_y > 272)
            vcursor_y = 272;
    }
}

const float GRAVITY_ACCELERATION = 28.0f;

auto test(glm::vec3 pos, World *wrld) -> bool {
    auto blk = wrld->worldData[wrld->getIdx(pos.x, pos.y, pos.z)];
    return blk != 0 && blk != 8 && blk != 6 && blk != 37 && blk != 38 &&
           blk != 39 && blk != 40 && blk != 10;
}

void Player::test_collide(glm::vec3 testpos, World *wrld, float dt) {
    model.ext = glm::vec3(0.6f, 1.8f, 0.6f);

    for (int x = -1; x <= 1; x++)
        for (int y = 0; y <= 4; y++)
            for (int z = -1; z <= 1; z++) {
                float xoff = x;
                float zoff = z;

                auto new_vec =
                    glm::vec3(testpos.x + xoff, testpos.y - 3.8f + (float)y,
                              testpos.z + zoff);

                if (test(new_vec, wrld)) {
                    AABB test_box =
                        AABB(glm::ivec3(new_vec.x, new_vec.y + 1, new_vec.z),
                             {1, 1, 1});

                    if (AABB::intersect(test_box, model)) {
                        float player_bottom = model.getMax().x;
                        float tiles_bottom = test_box.getMax().x;
                        float player_right = model.getMax().z;
                        float tiles_right = test_box.getMax().z;

                        float b_collision = tiles_bottom - model.getMin().x;
                        float t_collision = player_bottom - test_box.getMin().x;
                        float l_collision = player_right - test_box.getMin().z;
                        float r_collision = tiles_right - model.getMin().z;

                        if (t_collision < b_collision &&
                            t_collision < l_collision &&
                            t_collision < r_collision) {
                            // Top collision

                            if (vel.x > 0)
                                vel.x = 0.0f;
                        } else if (b_collision < t_collision &&
                                   b_collision < l_collision &&
                                   b_collision < r_collision) {
                            // bottom collision
                            if (vel.x < 0)
                                vel.x = 0.0f;
                        } else if (l_collision < r_collision &&
                                   l_collision < t_collision &&
                                   l_collision < b_collision) {
                            // Left collision
                            if (vel.z > 0)
                                vel.z = -0.0f;
                        } else if (r_collision < l_collision &&
                                   r_collision < t_collision &&
                                   r_collision < b_collision) {
                            // Right collision
                            if (vel.z < 0)
                                vel.z = 0.0f;
                        } else {
                            vel.x = 0.0f;
                            vel.z = 0.0f;
                        }
                    }
                }
            }

    testpos = pos + vel * dt;

    for (int x = -1; x <= 1; x++)
        for (int z = -1; z <= 1; z++) {
            if (test({testpos.x + (float)x, testpos.y - 1.8f,
                      testpos.z + (float)z},
                     wrld) &&
                vel.y < 0 && is_falling) {
                AABB test_box =
                    AABB(glm::ivec3(testpos.x + (float)x, testpos.y + 1,
                                    testpos.z + (float)z),
                         {1, 1, 1});
                if (AABB::intersect(test_box, model)) {
                    vel.y = 0;
                    is_falling = false;
                }
            }
        }

    if (test({testpos.x, testpos.y + 0.05f, testpos.z}, wrld)) {
        vel.y = 0;
        is_falling = true;
    }
}

void Player::update(float dt, World *wrld) {
    hasDir = false;
    rotate(dt, wrld->cfg.sense);
    jump_icd -= dt;

    // Update position
    if (!is_underwater)
        vel.y -= GRAVITY_ACCELERATION * dt;
    else
        vel.y -= GRAVITY_ACCELERATION * 0.3f * dt;
    is_falling = true;

    glm::vec3 testpos = pos + vel * dt;
    if (testpos.x < 0.5f || testpos.x > 255.0f) {
        vel.x = 0;
        testpos = pos + vel * dt;
    }
    if (testpos.z < 0.5f || testpos.z > 255.0f) {
        vel.z = 0;
        testpos = pos + vel * dt;
    }
    model.pos = testpos - glm::vec3(0.3f, 0, 0.3f);

    auto blk =
        wrld->worldData[wrld->getIdx(testpos.x, testpos.y + 0.2f, testpos.z)];
    if (blk == 8 || blk == 10)
        is_head_water = true;
    else
        is_head_water = false;

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.9f, testpos.z)];
    if (blk == 8 || blk == 10)
        is_underwater = true;
    else
        is_underwater = false;

    blk = wrld->worldData[wrld->getIdx(testpos.x, testpos.y - 1.2f, testpos.z)];
    if (blk == 8 || blk == 10)
        water_cutoff = true;
    else
        water_cutoff = false;

    test_collide(testpos, wrld, dt);

    if (is_underwater || is_head_water || water_cutoff) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.9f;
    }

    pos += vel * dt;

    auto diff = (pos.y - 1.80f) - static_cast<float>((int)(pos.y - 1.80f));
    if ((diff > 0.875f) && on_ground) {
        pos.y -= 1.80f;
        pos.y = roundf(pos.y);
        pos.y += 1.80f;
    }

    blk = wrld->worldData[wrld->getIdx(pos.x, pos.y - 1.85f, pos.z)];

    on_ground = (blk != 0 && blk != 8 && blk != 10);

    if (on_ground)
        if (jumping)
            jumping = false;

    if (vel.x != 0 || vel.z != 0) {
        view_timer += dt;
    } else {
        view_timer = 0;
    }
    view_bob = sinf(view_timer * 3.14159 * 2.5f) / 18.0f;
    cube_bob = cosf(view_timer * 3.14159 * 2.2f) / 44.0f;

    // Update camera
    cam.pos = pos;
    cam.pos.y += view_bob;
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
}

auto Player::drawBlk(uint8_t type, int x, int y) -> void {
    Rendering::RenderContext::get().matrix_view(glm::mat4(1));
    Rendering::RenderContext::get().matrix_translate(
        {153.5f + x * 20, 8 + y * 24, -20});
    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        Rendering::RenderContext::get().matrix_rotate({0.0f, 45.0f, 0});
    else
        Rendering::RenderContext::get().matrix_rotate({30.0f, 45.0f, 0});

    Rendering::RenderContext::get().matrix_scale({9.0f, 9.0f, 9.0f});

// DISABLE CULL
#if BUILD_PC
    glDisable(GL_CULL_FACE);
#else
    sceGuDisable(GU_CULL_FACE);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
    blockMesh[type].draw();

// ENABLE CULL
#if BUILD_PC
    glEnable(GL_CULL_FACE);
#else
    sceGuEnable(GU_CULL_FACE);
#endif

    Rendering::RenderContext::get().matrix_clear();
}

auto Player::drawBlkHand(uint8_t type) -> void {
    auto ctx = &Rendering::RenderContext::get();

    ctx->matrix_view(glm::mat4(1.0f));
    ctx->matrix_translate({0.280f, -0.7225f + cube_bob, -0.725f});
    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40 ||
        type == 44) {
        ctx->matrix_translate({0.0f, 0.175f, 0.0f});
    }
    ctx->matrix_rotate({0, 45.0f, 0});
    ctx->matrix_scale({0.40f, 0.40f, 0.40f});

    // DISABLE CULL
#if BUILD_PC
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
#else
    sceGuDisable(GU_CULL_FACE);
    sceGuClear(GU_DEPTH_BUFFER_BIT);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
    blockMesh[type].draw();

    // ENABLE CULL
#if BUILD_PC
    glEnable(GL_CULL_FACE);
#else
    sceGuEnable(GU_CULL_FACE);
#endif

    ctx->matrix_clear();
}

auto Player::draw() -> void {
    drawBlkHand(itemSelections[selectorIDX]);
    Rendering::RenderContext::get().set_mode_2D();
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, 100, -100);

    if (is_head_water) {
        water->set_position({0, 0});
        water->set_layer(1);
        water->draw();
    }

    if (!in_inventory) {
        crosshair->set_position({240 - 8, 136 - 8});
        crosshair->set_layer(-1);
        crosshair->draw();
    } else {
#if PSP
        crosshair->set_position({vcursor_x, vcursor_y});
        crosshair->set_layer(1);
        crosshair->draw();
#endif
    }

    if (in_inventory) {
        overlay->set_position({120 - 8, 64 - 44});
        overlay->set_layer(0);
        overlay->draw();
    }

    item_box->set_position({149, 1});
    item_box->set_layer(-1);
    item_box->draw();

    selector->set_position({148 + 20 * selectorIDX, 0});
    selector->set_layer(-2);
    selector->draw();

    textHelper->clear();
    textHelper->draw_text("Black Text", {200, 272 - 15}, CC_TEXT_COLOR_BLACK,
                          255, false);
    textHelper->draw_text("Dark Blue Text", {200, 272 - 24},
                          CC_TEXT_COLOR_DARK_BLUE, 255, false);
    textHelper->draw_text("Dark Green Text", {200, 272 - 33},
                          CC_TEXT_COLOR_DARK_GREEN, 255, false);
    textHelper->draw_text("Dark Aqua Text", {200, 272 - 42},
                          CC_TEXT_COLOR_DARK_AQUA, 255, false);
    textHelper->draw_text("Dark Red Text", {200, 272 - 51},
                          CC_TEXT_COLOR_DARK_RED, 255, false);
    textHelper->draw_text("Dark Purple Text", {200, 272 - 60},
                          CC_TEXT_COLOR_DARK_PURPLE, 255, false);
    textHelper->draw_text("Gold Text", {200, 272 - 69}, CC_TEXT_COLOR_GOLD, 255,
                          false);
    textHelper->draw_text("Gray Text", {200, 272 - 78}, CC_TEXT_COLOR_GRAY, 255,
                          false);
    textHelper->draw_text("Dark Gray Text", {200, 272 - 87},
                          CC_TEXT_COLOR_DARK_GRAY, 255, false);
    textHelper->draw_text("Blue Text", {200, 272 - 96}, CC_TEXT_COLOR_BLUE, 255,
                          false);
    textHelper->draw_text("Green Text", {200, 272 - 105}, CC_TEXT_COLOR_GREEN,
                          255, false);
    textHelper->draw_text("Aqua Text", {200, 272 - 114}, CC_TEXT_COLOR_AQUA,
                          255, false);
    textHelper->draw_text("Red Text", {200, 272 - 123}, CC_TEXT_COLOR_RED, 255,
                          false);
    textHelper->draw_text("Light Purple Text", {200, 272 - 132},
                          CC_TEXT_COLOR_LIGHT_PURPLE, 255, false);
    textHelper->draw_text("Yellow Text", {200, 272 - 141}, CC_TEXT_COLOR_YELLOW,
                          255, false);
    textHelper->draw_text("White Text", {200, 272 - 150}, CC_TEXT_COLOR_WHITE,
                          255, false);
    textHelper->draw_text("Bedrock MTX Gold Text", {200, 272 - 159},
                          CC_TEXT_COLOR_BE_MTX_GOLD, 255, false);

    textHelper->draw_text(
        "Position: " + std::to_string(static_cast<int>(pos.x)) + ", " +
            std::to_string(static_cast<int>(pos.y)) + ", " +
            std::to_string(static_cast<int>(pos.z)) + " ",
        {1, 272 - 10}, CC_TEXT_COLOR_WHITE, 255, true);

    textHelper->draw();

    for (int i = 0; i < 9; i++)
        drawBlk(itemSelections[i], i, 0);

    if (in_inventory) {
        for (int i = 0; i < 42; i++)
            drawBlk(inventorySelection[i], i % 9, 7 - i / 9);
    }
}
} // namespace CrossCraft
