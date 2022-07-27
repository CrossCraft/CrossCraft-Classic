#include "Player.hpp"
#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../MP/OutPackets.hpp"
#include "../TexturePackManager.hpp"
#include <Platform/Platform.hpp>
#include <Utilities/Input.hpp>
#include <Utilities/Logger.hpp>
#include <gtx/projection.hpp>

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

namespace CrossCraft {
template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

std::string chat_text;
Player *player_ptr;
#if BUILD_PC
void character_callback(GLFWwindow *window, unsigned int codepoint) {
    if (player_ptr != nullptr) {
        if (player_ptr->in_chat) {
            chat_text.push_back((char)codepoint);
        }
    }
}
#endif

auto Player::enter_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (!p->in_inventory) {
        if (p->in_chat) {
            chat_text = "";
        }

        p->in_chat = !p->in_chat;

#if BUILD_PC
        if (p->in_chat)
            glfwSetInputMode(Rendering::window, GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(Rendering::window, GLFW_CURSOR,
                             GLFW_CURSOR_DISABLED);

        Utilities::Input::set_cursor_center();
#endif
    }
}
auto Player::submit_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (p->client_ref != nullptr && chat_text.length() > 0) {
        auto ptr = create_refptr<MP::Outgoing::Message>();
        ptr->PacketID = MP::Outgoing::eMessage;
        memset(ptr->Message.contents, 0x20, STRING_LENGTH);
        memcpy((char *)ptr->Message.contents, chat_text.c_str(),
               chat_text.size() < STRING_LENGTH ? chat_text.size()
                                                : STRING_LENGTH);

        p->client_ref->packetsOut.push_back(
            MP::Outgoing::createOutgoingPacket(ptr.get()));
    }

    p->in_chat = false;
    chat_text = "";

#if BUILD_PC
    glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Utilities::Input::set_cursor_center();
#endif
}

auto Player::delete_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (p->in_chat) {
        if (chat_text.size() > 0) {
            chat_text.erase(chat_text.end() - 1);
        }
    }
}

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

    player_ptr = this;
    in_chat = false;
    client_ref = nullptr;
    chat_text = "";

    item_box = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{149, 1}, {182, 22}},
        Rendering::Rectangle{{0, (256.0f - 22.0f) / 256.0f},
                             {182.0f / 256.0f, 22.0f / 256.0f}});
    item_box->set_position({149, 1});
    item_box->set_layer(-1);

    selector = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{148, 0}, {24, 24}},
        Rendering::Rectangle{{0, (256.0f - 22.0f - 24.0f) / 256.0f},
                             {24.0f / 256.0f, 24.0f / 256.0f}});
    selector->set_layer(-2);

    crosshair = create_scopeptr<Graphics::G2D::Sprite>(
        gui_texture, Rendering::Rectangle{{240 - 8, 136 - 8}, {16, 16}},
        Rendering::Rectangle{
            {(256.0f - 16.0f) / 256.0f, (256.0f - 16.0f) / 256.0f},
            {16.0f / 256.0f, 16.0f / 256.0f}});
    crosshair->set_position({240 - 8, 136 - 8});
    crosshair->set_layer(-1);

    water = create_scopeptr<Graphics::G2D::Sprite>(
        water_texture, Rendering::Rectangle{{0, 0}, {480, 272}});
    water->set_position({0, 0});
    water->set_layer(1);

    playerHUD = create_scopeptr<UserInterface>();

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

    in_cursor_x = 0;
    in_cursor_y = 0;

    chat = create_scopeptr<Chat>();

    fps_count = 0;
    fps_timer = 0.0f;
    fps_display = 0;

#if BUILD_PC
    glfwSetCharCallback(Stardust_Celeste::Rendering::window,
                        character_callback);
#endif
}

const auto playerSpeed = 4.3f;

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

auto Player::add_face_to_mesh(std::array<float, 12> data,
                              std::array<float, 8> uv, uint32_t lightVal,
                              glm::vec3 mypos, uint8_t type)
    -> void { // Create color
#if BUILD_PLAT != BUILD_VITA
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
#endif
}

auto Player::setup_model(uint8_t type) -> void {
#if BUILD_PLAT != BUILD_VITA
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
#else
    auto uvs = getTexCoord(type, LIGHT_SIDE_X);
    blockRep[type] = create_scopeptr<Graphics::G2D::Sprite>(
        terrain_atlas, Rendering::Rectangle{{-1.5f, -4.0f}, {16, 16}},
        Rendering::Rectangle{{uvs[2], uvs[3]}, {-1.0f / 16.0f, -1.0f / 16.0f}});
    blockRep[type]->set_layer(-5);
#endif
}

auto Player::move_reset(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    p->vel.x = 0.0f;
    p->vel.z = 0.0f;
}

bool hasDir = false;

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling) &&
        !p->in_chat) {

        if (!hasDir) {
            p->vel.x = -sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
            p->vel.z = -cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
            hasDir = true;
        }
    }
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling) &&
        !p->in_chat) {

        if (!hasDir) {
            p->vel.x = sinf(DEGTORAD(-p->rot.y)) * playerSpeed;
            p->vel.z = cosf(DEGTORAD(-p->rot.y)) * playerSpeed;
            hasDir = true;
        }
    }
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && (p->is_underwater || !p->is_falling) &&
        !p->in_chat) {

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
    if (!p->in_inventory && (p->is_underwater || !p->is_falling) &&
        !p->in_chat) {

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
    if (!p->in_inventory && !p->in_chat) {
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

auto Player::press_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_y -= 1;
        if (p->in_cursor_y <= -1)
            p->in_cursor_y = 4;

        if (p->in_cursor_x >= 6 && p->in_cursor_y == 4)
            p->in_cursor_y = 3;
    } else {
        move_up(d);
    }
}

auto Player::press_down(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_y += 1;
        if (p->in_cursor_y >= 5)
            p->in_cursor_y = 0;

        if (p->in_cursor_x >= 6 && p->in_cursor_y == 4)
            p->in_cursor_y = 0;
    }
}

auto Player::press_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_x -= 1;

        if (p->in_cursor_x <= -1)
            if (p->in_cursor_y == 4)
                p->in_cursor_x = 5;
            else
                p->in_cursor_x = 8;
    } else {
        dec_selector(d);
    }
}

auto Player::press_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat) {
        p->in_cursor_x += 1;

        if (p->in_cursor_x >= 9 || (p->in_cursor_y == 4 && p->in_cursor_x >= 6))
            p->in_cursor_x = 0;
    } else {
        inc_selector(d);
    }
}

auto Player::change_selector(std::any d) -> void {
    auto s = std::any_cast<SelData>(d);
    s.player->selectorIDX = s.selIDX;
    s.player->selector->set_position({148 + 20 * s.player->selectorIDX, 0});
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX++;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;

    p->selector->set_position({148 + 20 * p->selectorIDX, 0});
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX--;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;

    p->selector->set_position({148 + 20 * p->selectorIDX, 0});
}

auto Player::toggle_inv(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_chat) {
        p->in_inventory = !p->in_inventory;

#if BUILD_PC
        if (p->in_inventory)
            glfwSetInputMode(Rendering::window, GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(Rendering::window, GLFW_CURSOR,
                             GLFW_CURSOR_DISABLED);

        Utilities::Input::set_cursor_center();
#endif
    }
}

auto Player::move_down(std::any d) -> void {
    // TODO: Sneak
}

auto Player::rotate(float dt, float sense) -> void {
    using namespace Utilities::Input;
    // Rotate player
    const auto rotSpeed = 500.0f;
    float cX, cY;

#if BUILD_PLAT == BUILD_VITA
    cX = get_axis("Vita", "LX");
    cY = get_axis("Vita", "LY");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;
#elif BUILD_PLAT == BUILD_PSP
    cX = get_axis("PSP", "X");
    cY = get_axis("PSP", "Y");

    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;
#else
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");

    cX *= 3.0f;
    cY *= 3.0f;
#endif
    if (!in_inventory && !in_chat) {
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
    }
}
#if BUILD_PLAT == BUILD_PSP

auto ShowOSK(unsigned short *descritpion, unsigned short *outtext,
             int maxtextinput) -> int {
    // osk params
    SceUtilityOskData oskData;
    SceUtilityOskParams oskParams;
    // init osk data
    memset(&oskData, 0, sizeof(SceUtilityOskData));
    oskData.language =
        PSP_UTILITY_OSK_LANGUAGE_DEFAULT; // Use system default for text input
    oskData.lines = 1;
    oskData.unk_24 = 1;
    oskData.inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL; // Allow all input types
    oskData.desc = descritpion;
    // oskData.intext = intext[i];
    oskData.outtextlength = maxtextinput;
    oskData.outtextlimit = 32; // Limit input to 32 characters
    oskData.outtext = outtext;

    // init osk dialog
    memset(&oskParams, 0, sizeof(SceUtilityOskParams));
    oskParams.base.size = sizeof(oskParams);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,
                                &oskParams.base.language);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,
                                &oskParams.base.buttonSwap);
    oskParams.base.graphicsThread = 17;
    oskParams.base.accessThread = 19;
    oskParams.base.fontThread = 18;
    oskParams.base.soundThread = 16;
    oskParams.datacount = 1;
    oskParams.data = &oskData;

    sceUtilityOskInitStart(&oskParams);

    bool done = true;

    while (done) {
        sceGuStart(GU_DIRECT, Stardust_Celeste::Rendering::list);
        sceGuClear(GU_COLOR_BUFFER_BIT);
        sceGuFinish();
        sceGuSync(0, 0);

        sceGuClear(GU_COLOR_BUFFER_BIT);

        switch (sceUtilityOskGetStatus()) {
        case PSP_UTILITY_DIALOG_INIT:
            break;

        case PSP_UTILITY_DIALOG_VISIBLE:
            sceUtilityOskUpdate(1);
            break;

        case PSP_UTILITY_DIALOG_QUIT:
            sceUtilityOskShutdownStart();
            break;

        case PSP_UTILITY_DIALOG_FINISHED:
            break;

        case PSP_UTILITY_DIALOG_NONE:
            done = false;

        default:
            break;
        }

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    if (oskData.result == PSP_UTILITY_OSK_RESULT_CANCELLED)
        return -1;

    return 0;
}
#endif

auto Player::psp_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

#if PSP
    if (p->client_ref != nullptr) {

        unsigned short test2[64];
        memset(test2, 0, 64 * sizeof(short));
        std::string message = "";

        unsigned short desc[5] = {'C', 'h', 'a', 't', '\0'};

        if (ShowOSK(desc, test2, 64) != -1) {
            for (int j = 0; test2[j]; j++) {
                unsigned c = test2[j];

                if (32 <= c && c <= 127) // print ascii only
                    message += c;
            }
        } else {
            return;
        }
        sceKernelDcacheWritebackInvalidateAll();

        auto ptr = create_refptr<MP::Outgoing::Message>();
        ptr->PacketID = MP::Outgoing::eMessage;
        memset(ptr->Message.contents, 0x20, STRING_LENGTH);
        memcpy(ptr->Message.contents, message.c_str(),
               chat_text.length() < STRING_LENGTH ? message.length()
                                                  : STRING_LENGTH);

        p->client_ref->packetsOut.push_back(
            MP::Outgoing::createOutgoingPacket(ptr.get()));

        SC_APP_INFO("Message Sent: {}", message);
    }

    p->in_chat = false;
#endif
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
    fps_timer += dt;
    fps_count++;
    if (fps_timer > 1.0f) {
        fps_timer = 0.0f;
        fps_display = fps_count;
        fps_count = 0;
    }

    chat->update(dt);
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
    cam.rot = glm::vec3(DEGTORAD(rot.x), DEGTORAD(rot.y), 0.f);
    cam.update();
}

auto Player::drawBlk(uint8_t type, int x, int y, float scale) -> void {
    Rendering::RenderContext::get().matrix_view(glm::mat4(1));
    Rendering::RenderContext::get().matrix_translate(
        {153.5f + x * 20, 8 + y * 24, -20});

#if BUILD_PLAT != BUILD_VITA
    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        Rendering::RenderContext::get().matrix_rotate({0.0f, 45.0f, 0});
    else
        Rendering::RenderContext::get().matrix_rotate({30.0f, 45.0f, 0});

    Rendering::RenderContext::get().matrix_scale({scale, scale, scale});
#else
    Rendering::RenderContext::get().matrix_scale(
        {scale / 9.0f, scale / 9.0f, scale / 9.0f});
#endif

// DISABLE CULL
#if BUILD_PC
    glDisable(GL_CULL_FACE);
#elif BUILD_PLAT == BUILD_VITA
    glDisable(GL_DEPTH_TEST);
#else
    sceGuDisable(GU_CULL_FACE);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
#if BUILD_PLAT != BUILD_VITA
    blockMesh[type].draw();

#else
    // if (type == Block::Cobblestone && blockRep[type] != nullptr)
    //     SC_APP_INFO("CBL");
    blockRep[type]->texture = terrain_atlas;
    blockRep[type]->draw();
#endif

// ENABLE CULL
#if BUILD_PC
    glEnable(GL_CULL_FACE);
#elif BUILD_PLAT == BUILD_VITA
    glEnable(GL_DEPTH_TEST);
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
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
#else
    sceGuDisable(GU_CULL_FACE);
    sceGuClear(GU_DEPTH_BUFFER_BIT);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
#if BUILD_PLAT != BUILD_VITA
    blockMesh[type].draw();
#endif

    // ENABLE CULL
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    glEnable(GL_CULL_FACE);
#else
    sceGuEnable(GU_CULL_FACE);
#endif

    ctx->matrix_clear();
}

auto Player::draw() -> void {
    int selectedBlock;
    selectedBlock = (in_cursor_x) + (in_cursor_y * 9);

    drawBlkHand(itemSelections[selectorIDX]);

    playerHUD->begin2D();

    if (is_head_water) {
        water->draw();
    }

    if (in_inventory) {
        playerHUD->draw_text("Select block", CC_TEXT_COLOR_WHITE,
                             CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_CENTER, 0, 7,
                             CC_TEXT_BG_NONE);
        playerHUD->draw_text(
            playerHUD->get_block_name(inventorySelection[selectedBlock]),
            CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM, 0,
            6, CC_TEXT_BG_NONE);
        Rendering::RenderContext::get().draw_rect({126, 55}, {226, 167},
                                                  {0, 0, 0, 128}, 2);
    } else {
        crosshair->draw();

        playerHUD->draw_text(
            playerHUD->get_block_name(itemSelections[selectorIDX]),
            CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_CENTER, CC_TEXT_ALIGN_BOTTOM, 0,
            3, CC_TEXT_BG_NONE);
    }

    item_box->draw();
    selector->draw();

    playerHUD->draw_text(
        "Position: " + std::to_string(static_cast<int>(pos.x)) + ", " +
            std::to_string(static_cast<int>(pos.y)) + ", " +
            std::to_string(static_cast<int>(pos.z)),
        CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT, CC_TEXT_ALIGN_TOP, 0, 0,
        CC_TEXT_BG_DYNAMIC);

    playerHUD->draw_text("FPS: " + std::to_string(fps_display),
                         CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_RIGHT,
                         CC_TEXT_ALIGN_TOP, 0, 0, false);

    int i = 5;
    for (int x = chat->data.size() - 1; x >= 0; x--) {
        auto &p = chat->data.at(x);
        if (i < 0)
            break;

        playerHUD->draw_text(p.text, CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT,
                             CC_TEXT_ALIGN_CENTER, 0, -i - 5,
                             CC_TEXT_BG_DYNAMIC);
        i--;
    }

    if (in_chat) {
        playerHUD->draw_text(chat_text, CC_TEXT_COLOR_WHITE, CC_TEXT_ALIGN_LEFT,
                             CC_TEXT_ALIGN_CENTER, 0, -11, CC_TEXT_BG_DYNAMIC);
    }

    playerHUD->end2D();

    for (int i = 0; i < 9; i++)
        drawBlk(itemSelections[i], i, 0, 9.0f);
    if (in_inventory) {
        for (int i = 0; i < 42; i++) {
            if (i == selectedBlock)
                drawBlk(inventorySelection[i], i % 9, 7 - i / 9,
#if BUILD_PLAT != BUILD_VITA
                        13.0f
#else
                        11.0f
#endif
                );
            else
                drawBlk(inventorySelection[i], i % 9, 7 - i / 9, 9.0f);
        }
    }
}

} // namespace CrossCraft
