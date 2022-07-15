#include "Menustate.hpp"
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/PSPController.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;

MenuState::~MenuState() { on_cleanup(); }

void MenuState::on_start() {
    TexturePackManager::get().scan_folder("./texturepacks/");

    // Make new controllers
    psp_controller = new Input::PSPController();
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

    logo_sprite->set_layer(1);

    gui_tex = TexturePackManager::get().load_texture(
        "assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    unsel_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 86.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});

    unsel_sprite->set_layer(1);

    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    fontRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));
    splashRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));

    splashRenderer->clear();
    splashRenderer->add_text("Classic!", {0, 0},
                             Rendering::Color{255, 255, 85, 255}, 2);
    splashRenderer->add_text("Classic!", {1, -1},
                             Rendering::Color{63, 63, 21, 255}, 2);
    splashRenderer->rebuild();
    scaleFactor = 1.3f;
    scaleTimer = 0.0f;
}

void MenuState::on_cleanup() {
    delete psp_controller;
    delete key_controller;
    delete mouse_controller;
}

void MenuState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void MenuState::on_update(Core::Application *app, double dt) {
    scaleTimer += dt;
    scaleFactor = 1.0f - (sinf(scaleTimer) * 0.3f);
}

const auto white = Rendering::Color{255, 255, 255, 255};
const auto shadow = Rendering::Color{63, 63, 63, 255};

void MenuState::on_draw(Core::Application *app, double dt) {
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 9; y++) {
            Rendering::RenderContext::get().matrix_translate(
                {x * 32, y * 32, 0});
            bg_tile->draw();
            Rendering::RenderContext::get().matrix_clear();
        }

    logo_sprite->draw();

    for (int i = 0; i < 4; i++) {
        Rendering::RenderContext::get().matrix_translate({0, -i * 28, 0});
        unsel_sprite->draw();
        Rendering::RenderContext::get().matrix_clear();
    }

    fontRenderer->clear();

    fontRenderer->add_text(
        "Singleplayer",
        {240 - fontRenderer->calculate_size("Singleplayer") / 2, 150}, white,
        2);
    fontRenderer->add_text(
        "Multiplayer",
        {240 - fontRenderer->calculate_size("Multiplayer") / 2, 150 - 28},
        white, 2);
    fontRenderer->add_text(
        "Texture Packs",
        {240 - fontRenderer->calculate_size("Texture Packs") / 2, 150 - 28 * 2},
        white, 2);
    fontRenderer->add_text(
        "Quit Game",
        {240 - fontRenderer->calculate_size("Quit Game") / 2, 150 - 28 * 3},
        white, 2);

    fontRenderer->add_text(
        "Singleplayer",
        {241 - fontRenderer->calculate_size("Singleplayer") / 2, 149}, shadow,
        2);
    fontRenderer->add_text(
        "Multiplayer",
        {241 - fontRenderer->calculate_size("Multiplayer") / 2, 149 - 28},
        shadow, 2);
    fontRenderer->add_text(
        "Texture Packs",
        {241 - fontRenderer->calculate_size("Texture Packs") / 2, 149 - 28 * 2},
        shadow, 2);
    fontRenderer->add_text(
        "Quit Game",
        {241 - fontRenderer->calculate_size("Quit Game") / 2, 149 - 28 * 3},
        shadow, 2);

    fontRenderer->rebuild();
    fontRenderer->draw();

    Rendering::RenderContext::get().matrix_rotate({0, 0, 30.0f});
    Rendering::RenderContext::get().matrix_translate({400, 16, 0});
    Rendering::RenderContext::get().matrix_scale(
        {scaleFactor, scaleFactor, 1.0f});
    splashRenderer->draw();
    Rendering::RenderContext::get().matrix_clear();
}

/* Ugly Key-Binding Function */

void MenuState::bind_controls() {
    Input::add_controller(psp_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);
}

} // namespace CrossCraft
