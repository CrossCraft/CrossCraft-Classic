#include "Player.hpp"
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../MP/OutPackets.hpp"
#include "../TexturePackManager.hpp"
#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/projection.hpp>
#include <gtx/rotate_vector.hpp>

#if PSP
#include <malloc.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <psputility.h>
#include <string.h>

namespace Stardust_Celeste::Rendering {
extern char list[0x100000] __attribute__((aligned(64)));
}
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

#if PSP
u32 totalRamFree() {
    u32 size = 0;
    const u32 BLOCK_SIZE = 1024 * 512;

    while (true) {
        size += BLOCK_SIZE;
        u8 *ram = (u8 *)malloc(size);

        if (ram == NULL) {
            size -= BLOCK_SIZE;
            break;
        }

        free(ram);
    }

    return size;
}
#endif

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

#if BUILD_PC
extern void character_callback(GLFWwindow *window, unsigned int codepoint);
#endif
extern std::string chat_text;
extern Player *player_ptr;

Player::Player()
    : pos(0.f, 64.0f, 0.f), rot(0.f, 180.f), vel(0.f, 0.f, 0.f),
      cam(pos, glm::vec3(rot.x, rot.y, 0), DEGTORAD(70.0f), 16.0f / 9.0f, 0.1f,
          255.0f),
      is_falling(true),
      model(pos, {0.6, 1.8, 0.6}), itemSelections{1,  4,  45, 3, 5,
                                                  17, 18, 20, 44},
      inventorySelection{1,  4,  45, 3,  5,  17, 18, 20, 44, 48, 6,
                         37, 38, 39, 40, 12, 13, 19, 21, 22, 23, 24,
                         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                         36, 14, 15, 16, 42, 41, 47, 46, 49} {
    gui_texture = TexturePackManager::get().load_texture(
        "assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);
    water_texture = TexturePackManager::get().load_texture(
        "assets/water.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, false,
        true);
    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    in_tab = false;
    player_ptr = this;
    in_chat = false;
    in_chat_delta = true;
    client_ref = nullptr;
    chat_text = "";
    in_pause = false;

    hasDir = false;

    background_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{126, 55}, {226, 167}},
        Rendering::Color{0, 0, 0, 128}, 2);

    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{149, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    item_box->set_position({149, 5});
    item_box->set_layer(-1);

    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{148, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});

    selector->set_position({148, 4});
    selector->set_layer(-2);

    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{240 - 8, 136 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    crosshair->set_layer(-1);

    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {480, 272}});
    water->set_position({0, 0});
    water->set_layer(1);

    playerHUD = create_scopeptr<UserInterface>();

    projmat = glm::perspective(DEGTORAD(70.0f), 16.0f / 9.0f, 0.1f, 255.0f);

    selectorIDX = 0;
    is_underwater = false;
    in_inventory = false;
    in_inv_delta = true;
    jump_icd = 0.2f;
    terrain_atlas = 0;
    view_timer = 0.0f;

    on_ground = false;
    jumping = false;

    in_cursor_x = 0;
    in_cursor_y = 0;

    chat = create_scopeptr<Chat>();
    blockRep = create_scopeptr<BlockRep>();
    pauseMenu = create_scopeptr<PauseMenu>();

    fps_count = 0;
    fps_timer = 0.0f;
    fps_display = 0;

#if BUILD_PC
    glfwSetCharCallback(Stardust_Celeste::Rendering::window,
                        character_callback);
#endif
}

auto Player::spawn(World *wrld) -> void {
    bool spawned = false;
    int count = 30;

    while (!spawned && count >= 0) {
        count--;
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

    pos = {128 + 0.5f, 40 + 1, 128 + 0.5f};
    pos.y += 1.8f;
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

    if (wrld->client != nullptr && wrld->client->disconnected)
        return;

    fps_timer += dt;
    fps_count++;
    if (fps_timer > 1.0f) {
        fps_timer = 0.0f;
        fps_display = fps_count;
        fps_count = 0;
    }

    chat->update(dt);

    hasDir = false;
    if (!in_pause)
        rotate(dt, wrld->cfg.sense);
    jump_icd -= dt;

    // Update position
    if (!is_underwater)
        vel.y -= GRAVITY_ACCELERATION * dt;
    else
        vel.y -= GRAVITY_ACCELERATION * 0.3f * dt;
    is_falling = true;

    glm::vec3 testpos = pos + vel * dt;
    if (testpos.x < 0.5f || testpos.x > wrld->world_size.x + 0.5f) {
        vel.x = 0;
        testpos = pos + vel * dt;
    }
    if (testpos.z < 0.5f || testpos.z > wrld->world_size.z + 0.5f) {
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

    auto vel2 = vel;
    if (is_underwater || is_head_water || water_cutoff) {
        vel.x *= 0.5f;
        vel.z *= 0.5f;
        vel.y *= 0.7f;
    }

    pos += vel * dt;
    vel = vel2;

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
    cube_bob = cosf(view_timer * 3.14159 * 4.8f) / 44.0f;

    // Update camera
    cam.pos = pos;
    cam.pos.y += view_bob;
    cam.pos.y -= (1.80f - 1.5965f);
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();

    viewmat = glm::mat4(1.0f);
    viewmat = glm::rotate(viewmat, DEGTORAD(rot.x), {1, 0, 0});
    viewmat = glm::rotate(viewmat, DEGTORAD(rot.y), {0, 1, 0});
    viewmat = glm::translate(viewmat, -pos);

    if (in_pause) {
        pauseMenu->update();
    }
}

auto Player::draw(World *wrld) -> void {
    int selectedBlock;
    selectedBlock = (in_cursor_x) + (in_cursor_y * 9);

    blockRep->terrain_atlas = terrain_atlas;

    if (wrld->client == nullptr || !wrld->client->disconnected)
        blockRep->drawBlkHand(itemSelections[selectorIDX], wrld, cube_bob);

    playerHUD->begin2D();

    auto ipos = glm::ivec3(static_cast<int>(pos.x), static_cast<int>(pos.y),
                           static_cast<int>(pos.z));

    bool change = in_inv_delta != in_inventory || in_chat_delta != in_chat ||
                  fps_count == 0 || prev_ipos != ipos ||
                  chat_size != chat->data.size() ||
                  selector_block_prev != selectedBlock ||
                  selector_idx_prev != selectorIDX ||
                  chat_text_size != chat_text.size() || in_tab ||
                  (wrld->client != nullptr && wrld->client->disconnected);

    if (change)
        playerHUD->clear();

    if (change) {
        if (wrld->client != nullptr) {
            if (wrld->client->disconnected) {
                playerHUD->draw_text(wrld->client->disconnectReason,
                                     CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER,
                                     CC_TEXT_ALIGN_CENTER, 0, 0,
                                     CC_TEXT_BG_NONE);
            }
        }
    }

    if (is_head_water) {
        water->draw();
    }

    if (change) {
        if (in_inventory) {
            playerHUD->draw_text("Select block", CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0,
                                 7, CC_TEXT_BG_NONE);
            playerHUD->draw_text(
                playerHUD->get_block_name(inventorySelection[selectedBlock]),
                CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM,
                0, 6, CC_TEXT_BG_NONE);
        } else {
            playerHUD->draw_text(
                playerHUD->get_block_name(itemSelections[selectorIDX]),
                CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM,
                0, 3, CC_TEXT_BG_NONE);
        }

        selector_block_prev = selectedBlock;
        selector_idx_prev = selectorIDX;
        in_inv_delta = in_inventory;
    }

    if (change && in_tab && client_ref != nullptr) {
        std::vector<std::string> names;
        for (auto &[key, val] : client_ref->player_rep) {
            names.push_back(val.name);
        }

        for (int i = 0; i < names.size(); i++) {
            playerHUD->draw_text(names[i], CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER,
                                 (i % 3) * 20 - 20, (-i / 3) + 10,
                                 CC_TEXT_BG_NONE);
        }
    }

    if (in_inventory || in_tab) {
        if (in_tab) {
            Rendering::RenderContext::get().matrix_scale({1.25f, 1.25f, 1.25f});
            Rendering::RenderContext::get().matrix_translate(
                {-46.0f, -20.875f, 0.0f});
            background_rectangle->draw();
            Rendering::RenderContext::get().matrix_clear();
        } else {
            background_rectangle->draw();
        }
    } else {
        crosshair->draw();
    }

    item_box->draw();
    selector->draw();

    if (change) {
        playerHUD->draw_text("Position: " + std::to_string((int)ipos.x) + ", " +
                                 std::to_string((int)ipos.y) + ", " +
                                 std::to_string((int)ipos.z),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT,
                             CC_TEXT_ALIGN_TOP, 0, 0, CC_TEXT_BG_DYNAMIC);

        prev_ipos = ipos;
    }

    if (change)
        playerHUD->draw_text("FPS: " + std::to_string(fps_display),
                             CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                             CC_TEXT_ALIGN_TOP, 0, 0, false);

    if (change) {
        int i = 9;
        for (int x = chat->data.size() - 1; x >= 0; x--) {
            auto &p = chat->data.at(x);
            if (i < 0)
                break;

            playerHUD->draw_text(p.text, CC_TEXT_COLOR_WHITE,
                                 CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_CENTER, 0,
                                 -i - 2, CC_TEXT_BG_NONE);
            i--;
        }
        chat_size = chat->data.size();
    }

    if (in_chat && change) {
        playerHUD->draw_text("> " + chat_text, CC_TEXT_COLOR_WHITE,
                             CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_BOTTOM, 0, 0, 5);
    }

    chat_text_size = chat_text.size();
    in_chat_delta = in_chat;

    if (change)
        playerHUD->rebuild();

    playerHUD->end2D();

    if (wrld->client == nullptr || !wrld->client->disconnected) {
        for (int i = 0; i < 9; i++)
            blockRep->drawBlk(itemSelections[i], i, 0, 4, 9.0f);
        if (in_inventory) {
            for (int i = 0; i < 42; i++) {
                if (i == selectedBlock)
                    blockRep->drawBlk(inventorySelection[i], i % 9, 7 - i / 9,
                                      0, 13.0f);
                else
                    blockRep->drawBlk(inventorySelection[i], i % 9, 7 - i / 9,
                                      0, 9.0f);
            }
        }
    }

    if (in_pause) {
        pauseMenu->draw();
    }
}

} // namespace CrossCraft
