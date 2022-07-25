#include "Menustate.hpp"
#include "../Gamestate.hpp"
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
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
#if BUILD_PLAT == BUILD_VITA
    sceIoMkdir("ux0:/data/CrossCraft-Classic", 0777);
    sceIoMkdir("ux0:/data/CrossCraft-Classic/texturepacks", 0777);

    {
        std::ifstream src("app0:/texturepacks/default.zip", std::ios::binary);
        std::ofstream dst(
            "ux0:/data/CrossCraft-Classic/texturepacks/default.zip",
            std::ios::binary);
        dst << src.rdbuf();
    }

    TexturePackManager::get().scan_folder(
        "ux0:/data/CrossCraft-Classic/texturepacks/");
#else
    TexturePackManager::get().scan_folder("./texturepacks/");
#endif
    textureMenu = false;

    // Make new controllers
    psp_controller = new Input::PSPController();
    vita_controller = new Input::VitaController();
    key_controller = new Input::KeyboardController();
    mouse_controller = new Input::MouseController();

    // Bind our controllers
    bind_controls();

    // Request 2D Mode
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, -30, 30);
    Rendering::RenderContext::get().set_mode_2D();

    bg_texture = TexturePackManager::get().load_texture(
        "assets/dirt.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, false,
        true);

    bg_tile = create_scopeptr<Graphics::G2D::Sprite>(
        bg_texture, Rendering::Rectangle{{0, 0}, {32, 32}},
        Rendering::Color{127, 127, 127, 255});

    logo_texture = TexturePackManager::get().load_texture(
        "assets/menu/logo.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    logo_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        logo_texture, Rendering::Rectangle{{-16, 272 - 72}, {512, 64}});

    logo_sprite->set_layer(-1);

    gui_tex = TexturePackManager::get().load_texture(
        "assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

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

    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

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

void MenuState::on_cleanup() {
    delete psp_controller;
    delete key_controller;
    delete mouse_controller;
    Input::clear_controller();

    Rendering::TextureManager::get().delete_texture(gui_tex);
    Rendering::TextureManager::get().delete_texture(font_texture);
    Rendering::TextureManager::get().delete_texture(bg_texture);
    Rendering::TextureManager::get().delete_texture(logo_texture);
}

void MenuState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void MenuState::on_update(Core::Application *app, double dt) {
    if (shouldQuit) {
        app->exit();
    }
    if (startSP) {
        app->set_state(create_refptr<GameState>());
        return;
    }
    if (startMP) {
        app->set_state(create_refptr<GameState>(true));
        return;
    }
    Utilities::Input::update();

    scaleTimer += dt;
    scaleFactor = 1.0f - (sinf(scaleTimer * 3.14159f) * 0.3f);

#if BUILD_PC
    selIdx = -1;
    float cX = Input::get_axis("Mouse", "X") * 480.0f;
    float cY = (1.0f - Input::get_axis("Mouse", "Y")) * 272.0f;

    if (!textureMenu) {
        if (cX >= 140.0f && cX <= 340.0f) {
            if (cY >= 144 && cY <= 164) {
                selIdx = 0;
            }
            if (cY >= 116 && cY <= 136) {
                selIdx = 1;
            }
            if (cY >= 88 && cY <= 108) {
                selIdx = 2;
            }
            if (cY >= 60 && cY <= 80) {
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

    fontRenderer->clear();
    splashRenderer->clear();
    if (!textureMenu) {
        fontRenderer->add_text(
            "Singleplayer",
            {241 - fontRenderer->calculate_size("Singleplayer") / 2, 149},
            shadow, -19);
        fontRenderer->add_text(
            "Multiplayer",
            {241 - fontRenderer->calculate_size("Multiplayer") / 2, 149 - 28},
            shadow, -19);
        fontRenderer->add_text(
            "Texture Packs",
            {241 - fontRenderer->calculate_size("Texture Packs") / 2,
             149 - 28 * 2},
            shadow, -19);
        fontRenderer->add_text(
            "Quit Game",
            {241 - fontRenderer->calculate_size("Quit Game") / 2, 149 - 28 * 3},
            shadow, -19);

        fontRenderer->add_text(
            "Singleplayer",
            {240 - fontRenderer->calculate_size("Singleplayer") / 2, 150},
            white, -20);
        fontRenderer->add_text(
            "Multiplayer",
            {240 - fontRenderer->calculate_size("Multiplayer") / 2, 150 - 28},
            white, -20);
        fontRenderer->add_text(
            "Texture Packs",
            {240 - fontRenderer->calculate_size("Texture Packs") / 2,
             150 - 28 * 2},
            white, -20);
        fontRenderer->add_text(
            "Quit Game",
            {240 - fontRenderer->calculate_size("Quit Game") / 2, 150 - 28 * 3},
            white, -20);

        splashRenderer->add_text("Classic!", {1, -1},
                                 Rendering::Color{63, 63, 21, 255}, -10);

        splashRenderer->add_text("Classic!", {0, 0},
                                 Rendering::Color{255, 255, 85, 255}, -11);

        logo_sprite->draw();

        for (int i = 0; i < 4; i++) {
            Rendering::RenderContext::get().matrix_translate({0, -i * 28, 0});
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

            if (TexturePackManager::get().path_names.size() > i) {
                auto name = TexturePackManager::get().path_names[i];

                Rendering::RenderContext::get().matrix_translate(
                    {0, -i * 24 + 50, 0});

                auto vec = TexturePackManager::get().layers;
                if (std::find(vec.begin(), vec.end(), name) != vec.end()) {
                    Rendering::RenderContext::get().matrix_translate(
                        {0, -50, 0});
                    dis_sprite->draw();
                } else {
                    if (selIdx == i + 1) {
                        sel_sprite->draw();
                    } else {
                        unsel_sprite->draw();
                    }
                }

                Rendering::RenderContext::get().matrix_clear();

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
            }
        }

        fontRenderer->add_text(
            "Back",
            {241 - fontRenderer->calculate_size("Back") / 2, 136 - 128 + 14},
            shadow, -19);

        fontRenderer->add_text(
            "Back",
            {240 - fontRenderer->calculate_size("Back") / 2, 136 - 128 + 14},
            white, -20);
    }
    fontRenderer->rebuild();
    splashRenderer->rebuild();

    fontRenderer->draw();

    if (!textureMenu) {
        Rendering::RenderContext::get().matrix_rotate({0, 0, 30.0f});
        Rendering::RenderContext::get().matrix_translate({400, 16, 0});
        Rendering::RenderContext::get().matrix_scale(
            {scaleFactor, scaleFactor, 1.0f});
        splashRenderer->draw();
        Rendering::RenderContext::get().matrix_clear();
    }

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
    sceGuDisable(GU_DEPTH_TEST);
#elif BUILD_PLAT == BUILD_VITA
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
        } else {
            auto name =
                TexturePackManager::get().path_names[mstate->selIdx - 1];
            auto &vec = TexturePackManager::get().layers;
            if (std::find(vec.begin(), vec.end(), name) == vec.end()) {
                vec.push_back(name);
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
    mstate->selIdx++;
    if (mstate->selIdx > 3)
        mstate->selIdx = 3;
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

    mouse_controller->add_command(
        {(int)Input::MouseButtons::Left, KeyFlag::Press},
        {MenuState::trigger, this});

    Input::add_controller(psp_controller);
    Input::add_controller(vita_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);
}
} // namespace CrossCraft
