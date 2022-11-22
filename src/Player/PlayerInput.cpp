#include "../BlockConst.hpp"
#include "../Chunk/ChunkUtil.hpp"
#include "../Controls.hpp"
#include "../Gamestate.hpp"
#include "../MP/OutPackets.hpp"
#include "../TexturePackManager.hpp"
#include "../World/SaveData.hpp"
#include "Player.hpp"
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

namespace GI {
extern unsigned int __attribute__((aligned(16))) list[0x10000];
}
#endif

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace GI {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {

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

auto Player::tab_start(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_pause)
        p->in_tab = true;
}
auto Player::tab_end(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->in_tab = false;
}

auto Player::enter_chat(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (!p->in_inventory && chat_text.size() == 0 && !p->in_pause) {
        if (p->in_chat) {
            chat_text = "";
        }

        p->in_chat = !p->in_chat;

#if BUILD_PC
        if (p->in_chat)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        Utilities::Input::set_cursor_center();
#endif
    }
}

auto Player::enter_chat_slash(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (!p->in_inventory && chat_text.size() == 0 && !p->in_pause) {
        if (p->in_chat) {
            chat_text = "";
        }

        p->in_chat = true;

#if BUILD_PC
        if (p->in_chat)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
auto Player::move_reset(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    p->vel.x = 0.0f;
    p->vel.z = 0.0f;
}

auto Player::move_forward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP
        if (!Controls::get().pspJoystickView) {
            p->rot.x -= 5.0f;
            if (p->rot.x < -89.9f) {
                p->rot.x = -89.9f;
            }

            if (p->rot.x > 89.9f) {
                p->rot.x = 89.9f;
            }
        } else {
            p->vel.x += -sinf(DEGTORAD(-p->rot.y));
            p->vel.z += -cosf(DEGTORAD(-p->rot.y));
        }
#else
        p->vel.x += -sinf(DEGTORAD(-p->rot.y));
        p->vel.z += -cosf(DEGTORAD(-p->rot.y));
#endif
    }
}

auto Player::move_backward(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP
        if (!Controls::get().pspJoystickView) {
            p->rot.x += 5.0f;
            if (p->rot.x < -89.9f) {
                p->rot.x = -89.9f;
            }

            if (p->rot.x > 89.9f) {
                p->rot.x = 89.9f;
            }
        } else {
            p->vel.x += sinf(DEGTORAD(-p->rot.y));
            p->vel.z += cosf(DEGTORAD(-p->rot.y));
        }
#elif BUILD_PLAT != BUILD_VITA
        p->vel.x += sinf(DEGTORAD(-p->rot.y));
        p->vel.z += cosf(DEGTORAD(-p->rot.y));
#endif
    } else if (p->in_pause) {
        if (p->in_pause) {
            if (p->pauseMenu->pauseState == 0) {
                if (p->pauseMenu->selIdx == 0) {
                    p->in_pause = false;
                    p->pauseMenu->exit();
                } else if (p->pauseMenu->selIdx == 1) {
                    p->pauseMenu->pauseState = 1;
                } else if (p->pauseMenu->selIdx == 2) {
                    SaveData::save(p->wrldRef);
                } else if (p->pauseMenu->selIdx == 3) {
                    exit(0);
                }
            } else if (p->pauseMenu->pauseState == 1) {
                if (p->pauseMenu->selIdx == 0) {
                    Option::get().music = !Option::get().music;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 1) {
                    Option::get().renderDist++;
                    if (Option::get().renderDist > 3) {
                        Option::get().renderDist = 0;
                    }
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 2) {
                    Option::get().bobbing = !Option::get().bobbing;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 3) {
                    Option::get().sound = !Option::get().sound;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 4) {
                    Option::get().fps = !Option::get().fps;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 5) {
                    Option::get().vsync = !Option::get().vsync;
                    Rendering::RenderContext::get().vsync = Option::get().vsync;
                    Option::get().writeOptions();
                } else if (p->pauseMenu->selIdx == 6) {
                    p->pauseMenu->pauseState = 2;
                } else if (p->pauseMenu->selIdx == 7) {
                    p->pauseMenu->pauseState = 0;
                }
            } else if (p->pauseMenu->pauseState == 2) {
                if (p->pauseMenu->selIdx == 0) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonBreak = val;
                    }
                } else if (p->pauseMenu->selIdx == 1) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonPlace = val;
                    }
                } else if (p->pauseMenu->selIdx == 2) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonMenu = val;
                    }
                } else if (p->pauseMenu->selIdx == 3) {
                    auto val = Controls::get().getNextKey();
                    if (val != 0) {
                        Controls::get().buttonJump = val;
                    }
                } else if (p->pauseMenu->selIdx == 4) {
#if PSP
                    Controls::get().pspJoystickView =
                        !Controls::get().pspJoystickView;
#elif BUILD_PLAT == BUILD_VITA
                    Controls::get().vitaJoystickSwap =
                        !Controls::get().vitaJoystickSwap;
#endif
                } else if (p->pauseMenu->selIdx == 5) {
                    p->pauseMenu->pauseState = 1;
                }
                Controls::get().writeControls();
                GameState::apply_controls();
            }
            delay(100);
            return;
        }
    }
}

auto Player::move_left(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {

#if PSP
        if (!Controls::get().pspJoystickView) {
            p->rot.y -= 5.0f;
            if (p->rot.y > 360.0f) {
                p->rot.y -= 360.0f;
            }

            if (p->rot.y < 0.0f) {
                p->rot.y += 360.0f;
            }

        } else {
            p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f));
            p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f));
        }
#else
        p->vel.x += -sinf(DEGTORAD(-p->rot.y + 90.f));
        p->vel.z += -cosf(DEGTORAD(-p->rot.y + 90.f));
#endif
    }
}

auto Player::move_right(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
#if PSP
        if (!Controls::get().pspJoystickView) {
            p->rot.y += 5.0f;
            if (p->rot.y > 360.0f) {
                p->rot.y -= 360.0f;
            }

            if (p->rot.y < 0.0f) {
                p->rot.y += 360.0f;
            }

        } else {
            p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f));
            p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f));
        }
#else
        p->vel.x += sinf(DEGTORAD(-p->rot.y + 90.f));
        p->vel.z += cosf(DEGTORAD(-p->rot.y + 90.f));
#endif
    }
}

auto Player::respawn(std::any d) -> void {
    auto p = std::any_cast<RespawnRequest>(d);

    if (!p.player->in_inventory && !p.player->in_chat && !p.wrld->client)
        p.player->spawn(p.wrld);
}

auto Player::move_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_chat && !p->in_pause) {
        if (!p->jumping && p->jump_icd < 0.0f && !p->is_underwater &&
            !p->is_falling) {
            p->vel.y = 8.4f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = false;
        }

        if (p->is_underwater && p->water_cutoff) {
            p->vel.y = 4.3f;
            p->jumping = true;
            p->jump_icd = 0.33f;
            p->is_falling = true;
        }
    }
}

auto Player::press_up(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (p->in_inventory && !p->in_chat && !p->in_pause) {
        p->in_cursor_y -= 1;
        if (p->in_cursor_y <= -1)
            p->in_cursor_y = 4;

        if (p->in_cursor_x >= 6 && p->in_cursor_y == 4)
            p->in_cursor_y = 3;
    } else if (p->in_pause) {
        p->pauseMenu->selIdx--;
        if (p->pauseMenu->selIdx < 0) {
            p->pauseMenu->selIdx = 0;
        }
    } else {
        move_up(d);
    }
}

auto Player::press_down(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_inventory && !p->in_pause) {
        p->psp_chat();
        p->in_chat = false;
    } else if (p->in_pause) {
        p->pauseMenu->selIdx++;

        if (p->pauseMenu->pauseState == 0) {
            if (p->pauseMenu->selIdx > 3) {
                p->pauseMenu->selIdx = 3;
            }
        } else if (p->pauseMenu->pauseState == 1) {
            if (p->pauseMenu->selIdx > 7) {
                p->pauseMenu->selIdx = 7;
            }
        } else if (p->pauseMenu->pauseState == 2) {
            if (p->pauseMenu->selIdx > 5) {
                p->pauseMenu->selIdx = 5;
            }
        }
    } else if (!p->in_chat && p->in_inventory) {
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
    if (!s.player->in_chat && !s.player->in_pause) {
        s.player->selectorIDX = s.selIDX;
        s.player->selector->set_position({148 + 20 * s.player->selectorIDX, 4});
    }
}

auto Player::pause(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);

    if (p->in_pause) {
        p->pauseMenu->exit();
        p->in_pause = false;
    } else {
        p->pauseMenu->enter();
        p->in_pause = true;
    }
}

auto Player::inc_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX++;

    if (p->selectorIDX > 8)
        p->selectorIDX = 0;

    p->selector->set_position({148 + 20 * p->selectorIDX, 4});
}

auto Player::dec_selector(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    p->selectorIDX--;

    if (p->selectorIDX < 0)
        p->selectorIDX = 8;

    p->selector->set_position({148 + 20 * p->selectorIDX, 4});
}

auto Player::toggle_inv(std::any d) -> void {
    auto p = std::any_cast<Player *>(d);
    if (!p->in_chat && !p->in_pause) {
        p->in_inventory = !p->in_inventory;

#if BUILD_PC
        if (p->in_inventory)
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    float mX, mY;

#if BUILD_PLAT == BUILD_VITA
    if (Controls::get().vitaJoystickSwap) {
        cX = get_axis("Vita", "LX");
        cY = get_axis("Vita", "LY");
    } else {
        cX = get_axis("Vita", "RX");
        cY = get_axis("Vita", "RY");
    }
    if (cX <= 0.4f && cX >= -0.4f)
        cX = 0.0f;
    if (cY <= 0.4f && cY >= -0.4f)
        cY = 0.0f;

    cX *= 0.3f;
    cY *= 0.3f;

    if (!Controls::get().vitaJoystickSwap) {
        mX = get_axis("Vita", "LX");
        mY = get_axis("Vita", "LY");
    } else {
        mX = get_axis("Vita", "RX");
        mY = get_axis("Vita", "RY");
    }

    if (mX > 0.5f || mX < -0.5f) {
        vel.x += sinf(DEGTORAD(-rot.y + 90.0f)) * mX;
        vel.z += cosf(DEGTORAD(-rot.y + 90.0f)) * mX;
    }

    if (mY > 0.5f || mY < -0.5f) {
        vel.x += sinf(DEGTORAD(-rot.y)) * mY;
        vel.z += cosf(DEGTORAD(-rot.y)) * mY;
    }

#elif BUILD_PLAT == BUILD_PSP
    cX = 0.0f;
    cY = 0.0f;
    if (Controls::get().pspJoystickView) {
        cX = get_axis("PSP", "X");
        cY = get_axis("PSP", "Y");

        if (cX <= 0.4f && cX >= -0.4f)
            cX = 0.0f;
        if (cY <= 0.4f && cY >= -0.4f)
            cY = 0.0f;

        cX *= 0.3f;
        cY *= 0.3f;
    } else {
        mX = get_axis("PSP", "X");
        mY = get_axis("PSP", "Y");

        if (mX > 0.5f || mX < -0.5f) {
            vel.x += sinf(DEGTORAD(-rot.y + 90.0f)) * mX;
            vel.z += cosf(DEGTORAD(-rot.y + 90.0f)) * mX;
        }

        if (mY > 0.5f || mY < -0.5f) {
            vel.x += sinf(DEGTORAD(-rot.y)) * mY;
            vel.z += cosf(DEGTORAD(-rot.y)) * mY;
        }
    }
#else
    cX = get_axis("Mouse", "X");
    cY = get_axis("Mouse", "Y");

    int rx, ry;
    glfwGetWindowSize(GI::window, &rx, &ry);
    cX *= (float)rx / 1024.0f;
    cX /= dt * 10.0f;
    cY *= (float)ry / 1024.0f;
    cY /= dt * 10.0f;
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
        sceGuStart(GU_DIRECT, GI::list);
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
#elif BUILD_PLAT == BUILD_VITA

auto ShowOSK(unsigned short *descritpion, unsigned short *outtext,
             int maxtextinput) -> int {

    SceImeDialogParam param;
    sceImeDialogParamInit(&param);

    param.supportedLanguages = 0x0001FFFF;
    param.languagesForced = true;
    param.type = SCE_IME_TYPE_BASIC_LATIN;
    param.title = descritpion;
    param.textBoxMode = SCE_IME_DIALOG_TEXTBOX_MODE_DEFAULT;
    param.inputTextBuffer = outtext;
    param.maxTextLength = maxtextinput;
    sceImeDialogInit(&param);

    bool done = true;

    int result = 0;
    while (done) {
        auto status = sceImeDialogGetStatus();

        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
            SceImeDialogResult res;
            memset(&res, 0, sizeof(SceImeDialogResult));
            sceImeDialogGetResult(&res);

            if (res.button != SCE_IME_DIALOG_BUTTON_ENTER) {
                result = -1;
            }
            done = false;
        }

        vglSwapBuffers(true);
    }

    sceImeDialogTerm();
    return result;
}
#endif

auto Player::psp_chat() -> void {

#if PSP
    if (client_ref != nullptr) {

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

        client_ref->packetsOut.push_back(
            MP::Outgoing::createOutgoingPacket(ptr.get()));

        SC_APP_INFO("Message Sent: {}", message);
    }
#elif BUILD_PLAT == BUILD_VITA
    if (client_ref != nullptr) {

        unsigned short test2[64];
        memset(test2, 0, 64 * sizeof(short));
        std::string message = "";

        unsigned short desc[5] = {'C', 'h', 'a', 't', '\0'};

        SC_APP_INFO("Calling OSK!");
        if (ShowOSK(desc, test2, 64) != -1) {
            for (int j = 0; test2[j] && j < 64; j++) {
                unsigned c = test2[j];

                if (32 <= c && c <= 127) // print ascii only
                    message += c;
            }
            SC_APP_INFO("RES {}", message);
        } else {
            SC_APP_INFO("FAILED!");
            return;
        }

        auto ptr = create_refptr<MP::Outgoing::Message>();
        ptr->PacketID = MP::Outgoing::eMessage;
        memset(ptr->Message.contents, 0x20, STRING_LENGTH);
        memcpy(ptr->Message.contents, message.c_str(),
               chat_text.length() < STRING_LENGTH ? message.length()
                                                  : STRING_LENGTH);

        client_ref->packetsOut.push_back(
            MP::Outgoing::createOutgoingPacket(ptr.get()));

        SC_APP_INFO("Message Sent: {}", message);
    }
#endif
}

} // namespace CrossCraft
