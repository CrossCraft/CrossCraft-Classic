#include "Menustate.hpp"
#include "../Gamestate.hpp"
#include "../MusicManager.hpp"
#include "../UI/TextHelper.hpp"
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/N3DSController.hpp>
#include <Utilities/Controllers/PSPController.hpp>
#include <Utilities/Controllers/VitaController.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;

const auto white = Rendering::Color{255, 255, 255, 255};
const auto shadow = Rendering::Color{63, 63, 63, 255};

MenuState::~MenuState() { on_cleanup(); }

void MenuState::on_start() {
    createDirs();
    TexturePackManager::get().scan_folder(PLATFORM_FILE_PREFIX +
                                          "texturepacks/");
    textureMenu = false;

    // Make new controllers
    psp_controller = new Input::PSPController();
    vita_controller = new Input::VitaController();
    key_controller = new Input::KeyboardController();
    mouse_controller = new Input::MouseController();
    n3ds_controller = new Input::N3DSController();

    // Bind our controllers
    bind_controls();

    // Request 2D Mode
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, -30, 30);
    Rendering::RenderContext::get().set_mode_2D();

    bg_texture = TexturePackManager::get().load_texture(
        "assets/dirt.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, false,
        true);

    logo_texture = TexturePackManager::get().load_texture(
        "assets/menu/logo.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    gui_tex = TexturePackManager::get().load_texture(
        "assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    bg_tile = create_scopeptr<Graphics::G2D::Sprite>(
        bg_texture, Rendering::Rectangle{{0, 0}, {32, 32}},
        Rendering::Color{80, 80, 80, 255});

    logo_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        logo_texture, Rendering::Rectangle{{-16, 272 - 87}, {512, 64}});

    logo_sprite->set_layer(-1);

    unsel_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 86.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});

    unsel_sprite->set_layer(-1);

    sel_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 106.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});
    sel_sprite->set_layer(-1);

    dis_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 194}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 66.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});
    dis_sprite->set_layer(-1);

    fontRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));
    splashRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));

    scaleFactor = 1.3f;
    scaleTimer = 0.0f;

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
    selIdx = 0;
#endif
}

void MenuState::on_cleanup() {}

void MenuState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void MenuState::on_update(Core::Application *app, double dt) {
    MusicManager::get().update(dt);
    if (shouldQuit) {
        app->exit();
    }
    if (startSP) {
        SC_APP_INFO("STARTING GAME");

        Input::clear_controller();

        delete psp_controller;
        delete key_controller;
        delete mouse_controller;
        delete vita_controller;
        delete n3ds_controller;

        app->push_state(create_refptr<GameState>());
        return;
    }
    if (startMP) {
        SC_APP_INFO("STARTING GAME");

        Input::clear_controller();

        delete psp_controller;
        delete key_controller;
        delete mouse_controller;
        delete vita_controller;
        delete n3ds_controller;

        app->push_state(create_refptr<GameState>(true));
        return;
    }
    Utilities::Input::update();

    scaleTimer += dt * 4.6;
    scaleFactor = 1.75f - (sinf(scaleTimer * 3.14159f) * 0.03f);

#if BUILD_PC
    selIdx = -1;
    float cX = Input::get_axis("Mouse", "X") * 480.0f;
    float cY = (1.0f - Input::get_axis("Mouse", "Y")) * 272.0f;

    if (!textureMenu) {
        if (cX >= 140.0f && cX <= 340.0f) {
            if (cY >= 127 && cY <= 147) {
                selIdx = 0;
            }
            if (cY >= 105 && cY <= 125) {
                selIdx = 1;
            }
            if (cY >= 80 && cY <= 100) {
                selIdx = 2;
            }
            if (cY >= 56 && cY <= 76) {
                selIdx = 3;
            }
        }
    } else {
        if (cX >= 140.0f && cX <= 340.0f) {
            if (cY >= 16 && cY <= 36) {
                selIdx = 0;
            } else {
                cY -= 24;
                if (cY >= 174 && cY < 194) {
                    selIdx = 1;
                } else if (cY >= 150 && cY < 170) {
                    selIdx = 2;
                } else if (cY >= 126 && cY < 146) {
                    selIdx = 3;
                } else if (cY >= 102 && cY < 122) {
                    selIdx = 4;
                } else if (cY >= 78 && cY < 98) {
                    selIdx = 5;
                } else if (cY >= 54 && cY < 74) {
                    selIdx = 6;
                }
            }
        }
    }

#endif
}

void MenuState::on_draw(Core::Application *app, double dt) {

    Rendering::RenderContext::get().set_mode_2D();
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, 100, -100);

    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 9; y++) {
            Rendering::RenderContext::get().matrix_translate(
                {x * 32, y * 32, 0});
            bg_tile->draw();
            Rendering::RenderContext::get().matrix_clear();
        }

    fontRenderer->clear_tiles();
    splashRenderer->clear_tiles();
    if (!textureMenu) {
        // Singleplayer
        if (selIdx != 0) {
            fontRenderer->add_text(
                "Singleplayer",
                {241 - fontRenderer->calculate_size("Singleplayer") / 2, 134},
                shadow, -19);
            fontRenderer->add_text(
                "Singleplayer",
                {240 - fontRenderer->calculate_size("Singleplayer") / 2, 135},
                white, -20);
        } else {
            fontRenderer->add_text(
                "Singleplayer",
                {241 - fontRenderer->calculate_size("Singleplayer") / 2, 134},
                CC_TEXT_COLOR_SELECT_BACK, -19);
            fontRenderer->add_text(
                "Singleplayer",
                {240 - fontRenderer->calculate_size("Singleplayer") / 2, 135},
                CC_TEXT_COLOR_SELECT_FRONT, -20);
        }

        // Multiplayer
        if (selIdx != 1) {
            fontRenderer->add_text(
                "Multiplayer",
                {241 - fontRenderer->calculate_size("Multiplayer") / 2,
                 134 - 24},
                shadow, -19);
            fontRenderer->add_text(
                "Multiplayer",
                {240 - fontRenderer->calculate_size("Multiplayer") / 2,
                 135 - 24},
                white, -20);
        } else {
            fontRenderer->add_text(
                "Multiplayer",
                {241 - fontRenderer->calculate_size("Multiplayer") / 2,
                 134 - 24},
                CC_TEXT_COLOR_SELECT_BACK, -19);
            fontRenderer->add_text(
                "Multiplayer",
                {240 - fontRenderer->calculate_size("Multiplayer") / 2,
                 135 - 24},
                CC_TEXT_COLOR_SELECT_FRONT, -20);
        }

        // Texture Packs
        if (selIdx != 2) {
            fontRenderer->add_text(
                "Texture Packs",
                {241 - fontRenderer->calculate_size("Texture Packs") / 2,
                 134 - 24 * 2},
                shadow, -19);
            fontRenderer->add_text(
                "Texture Packs",
                {240 - fontRenderer->calculate_size("Texture Packs") / 2,
                 135 - 24 * 2},
                white, -20);
        } else {
            fontRenderer->add_text(
                "Texture Packs",
                {241 - fontRenderer->calculate_size("Texture Packs") / 2,
                 134 - 24 * 2},
                CC_TEXT_COLOR_SELECT_BACK, -19);
            fontRenderer->add_text(
                "Texture Packs",
                {240 - fontRenderer->calculate_size("Texture Packs") / 2,
                 135 - 24 * 2},
                CC_TEXT_COLOR_SELECT_FRONT, -20);
        }

        // Quit Game
        if (selIdx != 3) {
            fontRenderer->add_text(
                "Quit Game",
                {241 - fontRenderer->calculate_size("Quit Game") / 2,
                 134 - 24 * 3},
                shadow, -19);
            fontRenderer->add_text(
                "Quit Game",
                {240 - fontRenderer->calculate_size("Quit Game") / 2,
                 135 - 24 * 3},
                white, -20);
        } else {
            fontRenderer->add_text(
                "Quit Game",
                {241 - fontRenderer->calculate_size("Quit Game") / 2,
                 134 - 24 * 3},
                CC_TEXT_COLOR_SELECT_BACK, -19);
            fontRenderer->add_text(
                "Quit Game",
                {240 - fontRenderer->calculate_size("Quit Game") / 2,
                 135 - 24 * 3},
                CC_TEXT_COLOR_SELECT_FRONT, -20);
        }

        fontRenderer->add_text("CrossCraft Classic 1.3.0-pre1 (0.30)",
                               {3, 272 - 11}, Rendering::Color{21, 21, 21, 255},
                               -19);
        fontRenderer->add_text("CrossCraft Classic 1.3.0-pre1 (0.30)",
                               {2, 272 - 10}, Rendering::Color{85, 85, 85, 255},
                               -20);

        fontRenderer->add_text(
            "Copyleft CrossCraft Team. Made with <3!",
            {478 - fontRenderer->calculate_size(
                       "Copyleft CrossCraft Team. Made with <3!"),
             2},
            Rendering::Color{63, 63, 63, 255}, -19);
        fontRenderer->add_text(
            "Copyleft CrossCraft Team. Made with <3!",
            {477 - fontRenderer->calculate_size(
                       "Copyleft CrossCraft Team. Made with <3!"),
             3},
            Rendering::Color{255, 255, 255, 255}, -19);

        // Splash text uses pure yellow instead of registered yellow for..
        // some reason..
        splashRenderer->add_text("Classic!", {1, -1},
                                 CC_TEXT_COLOR_SPLASH_FRONT, -10);

        splashRenderer->add_text("Classic!", {0, 0}, CC_TEXT_COLOR_SPLASH_BACK,
                                 -11);

        logo_sprite->draw();

        for (int i = 0; i < 4; i++) {
            Rendering::RenderContext::get().matrix_translate(
                {0, (-i * 24) - 15, 0});
            if (selIdx == i)
                sel_sprite->draw();
            else
                unsel_sprite->draw();
            Rendering::RenderContext::get().matrix_clear();
        }
    } else {
        Rendering::RenderContext::get().matrix_translate({0, -128, 0});
        if (selIdx == 0)
            sel_sprite->draw();
        else
            unsel_sprite->draw();
        Rendering::RenderContext::get().matrix_clear();

        fontRenderer->add_text(
            "Texture Packs:",
            {241 - fontRenderer->calculate_size("Texture Packs:") / 2, 240},
            shadow, -19);

        fontRenderer->add_text(
            "Texture Packs:",
            {240 - fontRenderer->calculate_size("Texture Packs:") / 2, 240},
            white, -20);

        for (int i = 0; i < 6; i++) {
            bool do_not_select;
            if (TexturePackManager::get().path_names.size() > i) {
                auto name = TexturePackManager::get().path_names[i];

                Rendering::RenderContext::get().matrix_translate(
                    {0, -i * 24 + 50, 0});

                auto vec = TexturePackManager::get().layers;
                if (std::find(vec.begin(), vec.end(), name) != vec.end()) {
                    Rendering::RenderContext::get().matrix_translate(
                        {0, -50, 0});
                    dis_sprite->draw();
                    do_not_select = true;
                } else {
                    do_not_select = false;
                    if (selIdx == i + 1) {
                        sel_sprite->draw();
                    } else {
                        unsel_sprite->draw();
                    }
                }

                Rendering::RenderContext::get().matrix_clear();

                if (selIdx != i + 1 || do_not_select == true) {
                    fontRenderer->add_text(
                        name,
                        {241 - fontRenderer->calculate_size(name) / 2,
                         200 - i * 24},
                        shadow, -19);
                    fontRenderer->add_text(
                        name,
                        {240 - fontRenderer->calculate_size(name) / 2,
                         200 - i * 24},
                        white, -20);
                } else {
                    fontRenderer->add_text(
                        name,
                        {241 - fontRenderer->calculate_size(name) / 2,
                         200 - i * 24},
                        CC_TEXT_COLOR_SELECT_BACK, -19);
                    fontRenderer->add_text(
                        name,
                        {240 - fontRenderer->calculate_size(name) / 2,
                         200 - i * 24},
                        CC_TEXT_COLOR_SELECT_FRONT, -20);
                }
            }
        }

        if (selIdx != 0) {
            fontRenderer->add_text(
                "Back",
                {241 - fontRenderer->calculate_size("Back") / 2,
                 136 - 128 + 14},
                shadow, -19);
            fontRenderer->add_text(
                "Back",
                {240 - fontRenderer->calculate_size("Back") / 2,
                 136 - 128 + 14},
                white, -20);
        } else {
            fontRenderer->add_text(
                "Back",
                {241 - fontRenderer->calculate_size("Back") / 2,
                 136 - 128 + 14},
                CC_TEXT_COLOR_SELECT_BACK, -19);
            fontRenderer->add_text(
                "Back",
                {240 - fontRenderer->calculate_size("Back") / 2,
                 136 - 128 + 14},
                CC_TEXT_COLOR_SELECT_FRONT, -20);
        }
    }
    fontRenderer->generate_map();
    splashRenderer->generate_map();

    fontRenderer->draw();

    if (!textureMenu) {
        Rendering::RenderContext::get().matrix_rotate({0, 0, 22.0f});
        Rendering::RenderContext::get().matrix_translate({360, 50, 0});
        Rendering::RenderContext::get().matrix_scale(
            {scaleFactor, scaleFactor, 1.75f});
        splashRenderer->draw();
        Rendering::RenderContext::get().matrix_clear();
    }

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
    sceGuDisable(GU_DEPTH_TEST);
#else
    glDisable(GL_DEPTH_TEST);
#endif
}

void MenuState::trigger(std::any m) {
    auto mstate = std::any_cast<MenuState *>(m);

    if (!mstate->textureMenu) {
        if (mstate->selIdx == 0) {
            mstate->startSP = true;
        }
        if (mstate->selIdx == 1) {
            mstate->startMP = true;
        }
        if (mstate->selIdx == 2) {
            mstate->textureMenu = true;
            mstate->selIdx = 0;
        }
        if (mstate->selIdx == 3) {
            mstate->shouldQuit = true;
        }
    } else {
        if (mstate->selIdx == 0) {
            mstate->textureMenu = false;
        } else if (mstate->selIdx != -1) {
            auto name =
                TexturePackManager::get().path_names[mstate->selIdx - 1];
            auto &vec = TexturePackManager::get().layers;
            if (std::find(vec.begin(), vec.end(), name) == vec.end()) {
                vec.push_back(name);

                Rendering::TextureManager::get().delete_texture(
                    mstate->bg_texture);
                Rendering::TextureManager::get().delete_texture(
                    mstate->logo_texture);
                Rendering::TextureManager::get().delete_texture(
                    mstate->gui_tex);
                Rendering::TextureManager::get().delete_texture(
                    mstate->font_texture);

                mstate->bg_texture = TexturePackManager::get().load_texture(
                    "assets/dirt.png", SC_TEX_FILTER_NEAREST,
                    SC_TEX_FILTER_NEAREST, false, true);

                mstate->logo_texture = TexturePackManager::get().load_texture(
                    "assets/menu/logo.png", SC_TEX_FILTER_NEAREST,
                    SC_TEX_FILTER_NEAREST, false, true);

                mstate->gui_tex = TexturePackManager::get().load_texture(
                    "assets/gui/gui.png", SC_TEX_FILTER_NEAREST,
                    SC_TEX_FILTER_NEAREST, false, true);

                mstate->font_texture = TexturePackManager::get().load_texture(
                    "assets/default.png", SC_TEX_FILTER_NEAREST,
                    SC_TEX_FILTER_NEAREST, false, false);

                mstate->bg_tile->texture = mstate->bg_texture;
                mstate->logo_sprite->texture = mstate->logo_texture;
                mstate->unsel_sprite->texture = mstate->gui_tex;
                mstate->sel_sprite->texture = mstate->gui_tex;
                mstate->dis_sprite->texture = mstate->gui_tex;

                mstate->fontRenderer->texture = mstate->font_texture;
                mstate->splashRenderer->texture = mstate->font_texture;

            } else {
                if (name != "default")
                    vec.erase(std::find(vec.begin(), vec.end(), name));
            }
        }
    }
}

void MenuState::up(std::any m) {
    auto mstate = std::any_cast<MenuState *>(m);
    mstate->selIdx--;
    if (mstate->selIdx < 0)
        mstate->selIdx = 0;
}
void MenuState::down(std::any m) {
    auto mstate = std::any_cast<MenuState *>(m);

    if (!mstate->textureMenu) {
        mstate->selIdx++;
        if (mstate->selIdx > 3)
            mstate->selIdx = 3;
    } else {

        mstate->selIdx++;
        int total_idx = TexturePackManager::get().path_names.size();
        if (total_idx > 6)
            total_idx = 6;

        total_idx + 1;

        if (mstate->selIdx > total_idx)
            mstate->selIdx = 0;
    }
}

/* Ugly Key-Binding Function */

void MenuState::bind_controls() {

    psp_controller->add_command({(int)Input::PSPButtons::Cross, KeyFlag::Press},
                                {MenuState::trigger, this});
    psp_controller->add_command({(int)Input::PSPButtons::Up, KeyFlag::Press},
                                {MenuState::up, this});
    psp_controller->add_command({(int)Input::PSPButtons::Down, KeyFlag::Press},
                                {MenuState::down, this});

    vita_controller->add_command(
        {(int)Input::VitaButtons::Cross, KeyFlag::Press},
        {MenuState::trigger, this});
    vita_controller->add_command({(int)Input::VitaButtons::Up, KeyFlag::Press},
                                 {MenuState::up, this});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Down, KeyFlag::Press},
        {MenuState::down, this});

    n3ds_controller->add_command({(int)Input::N3DSButtons::A, KeyFlag::Press},
                                 {MenuState::trigger, this});
    n3ds_controller->add_command({(int)Input::N3DSButtons::Up, KeyFlag::Press},
                                 {MenuState::up, this});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Down, KeyFlag::Press},
        {MenuState::down, this});

    mouse_controller->add_command(
        {(int)Input::MouseButtons::Left, KeyFlag::Press},
        {MenuState::trigger, this});

    Input::add_controller(psp_controller);
    Input::add_controller(vita_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);
    Input::add_controller(n3ds_controller);
}
} // namespace CrossCraft
