#include "PauseMenu.hpp"
#include "../TexturePackManager.hpp"
#include "../UI/TextHelper.hpp"

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Stardust_Celeste::Rendering {
extern GLFWwindow *window;
}
#endif

namespace CrossCraft {

PauseMenu::PauseMenu() {
    gui_tex = TexturePackManager::get().load_texture(
        "assets/gui/gui.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, true);

    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    unsel_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 86.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});

    unsel_sprite->set_layer(-4);

    sel_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 106.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});
    sel_sprite->set_layer(-4);

    dis_sprite = create_scopeptr<Graphics::G2D::Sprite>(
        gui_tex, Rendering::Rectangle{{140, 144}, {200, 20}},
        Rendering::Rectangle{{0, (256.0f - 66.0f) / 256.0f},
                             {200.0f / 256.0f, 20.0f / 256.0f}});
    dis_sprite->set_layer(-4);

    fontRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));

    background_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{0, 0}, {480, 272}}, Rendering::Color{0, 0, 0, 96},
        -3);
}
PauseMenu::~PauseMenu() {}

auto PauseMenu::enter() -> void {
#if BUILD_PC
    glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif

    selIdx = 0;
}
auto PauseMenu::exit() -> void {
#if BUILD_PC
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Utilities::Input::set_cursor_center();
#endif

    selIdx = 0;
}

auto PauseMenu::draw() -> void {

#if BUILD_PC
    glClear(GL_DEPTH_BUFFER_BIT);
#endif

    background_rectangle->draw();

    Rendering::RenderContext::get().matrix_translate({0, 10, 0});

    if (selIdx == 0)
        sel_sprite->draw();
    else
        unsel_sprite->draw();

    Rendering::RenderContext::get().matrix_translate({0, -24, 0});
    dis_sprite->draw();

    Rendering::RenderContext::get().matrix_translate({0, -24, 0});
    if (selIdx == 1)
        sel_sprite->draw();
    else
        unsel_sprite->draw();

    Rendering::RenderContext::get().matrix_translate({0, -48, 0});
    if (selIdx == 2)
        sel_sprite->draw();
    else
        unsel_sprite->draw();

    Rendering::RenderContext::get().matrix_clear();

    fontRenderer->clear();

    fontRenderer->add_text(
        "Game Menu", {240 - fontRenderer->calculate_size("Game Menu") / 2, 216},
        {255, 255, 255, 255}, -11);
    fontRenderer->add_text(
        "Game Menu", {241 - fontRenderer->calculate_size("Game Menu") / 2, 215},
        {63, 63, 63, 255}, -10);

    if (selIdx != 0) {
        fontRenderer->add_text(
            "Back to Game",
            {240 - fontRenderer->calculate_size("Back to Game") / 2, 160},
            {255, 255, 255, 255}, -11);
        fontRenderer->add_text(
            "Back to Game",
            {241 - fontRenderer->calculate_size("Back to Game") / 2, 159},
            {63, 63, 63, 255}, -10);
    } else {
        fontRenderer->add_text(
            "Back to Game",
            {240 - fontRenderer->calculate_size("Back to Game") / 2, 160},
            CC_TEXT_COLOR_SELECT_FRONT, -12);
        fontRenderer->add_text(
            "Back to Game",
            {241 - fontRenderer->calculate_size("Back to Game") / 2, 159},
            CC_TEXT_COLOR_SELECT_BACK, -10);
    }

    fontRenderer->add_text(
        "Options",
        {240 - fontRenderer->calculate_size("Options") / 2, 160 - 24},
        {255, 255, 255, 255}, -11);
    fontRenderer->add_text(
        "Options",
        {241 - fontRenderer->calculate_size("Options") / 2, 160 - 25},
        {63, 63, 63, 255}, -10);

    if (selIdx != 1) {
        fontRenderer->add_text(
            "Save Game",
            {240 - fontRenderer->calculate_size("Save Game") / 2, 160 - 48},
            {255, 255, 255, 255}, -11);
        fontRenderer->add_text(
            "Save Game",
            {241 - fontRenderer->calculate_size("Save Game") / 2, 160 - 49},
            {63, 63, 63, 255}, -10);
    } else {
        fontRenderer->add_text(
            "Save Game",
            {240 - fontRenderer->calculate_size("Save Game") / 2, 160 - 48},
            CC_TEXT_COLOR_SELECT_FRONT, -11);
        fontRenderer->add_text(
            "Save Game",
            {241 - fontRenderer->calculate_size("Save Game") / 2, 160 - 49},
            CC_TEXT_COLOR_SELECT_BACK, -10);
    }

    if (selIdx != 2) {
        fontRenderer->add_text(
            "Quit Game", {240 - fontRenderer->calculate_size("Quit Game") / 2, 64},
            {255, 255, 255, 255}, -11);
        fontRenderer->add_text(
            "Quit Game", {241 - fontRenderer->calculate_size("Quit Game") / 2, 63},
            {63, 63, 63, 255}, -10);
    } else {
        fontRenderer->add_text(
            "Quit Game", {240 - fontRenderer->calculate_size("Quit Game") / 2, 64},
            CC_TEXT_COLOR_SELECT_FRONT, -11);
        fontRenderer->add_text(
            "Quit Game", {241 - fontRenderer->calculate_size("Quit Game") / 2, 63},
            CC_TEXT_COLOR_SELECT_BACK, -10);
    }


    fontRenderer->rebuild();
    fontRenderer->draw();
}
auto PauseMenu::update() -> void {
#if BUILD_PC
    selIdx = -1;
    float cX =
        Utilities::Input::get_axis("Mouse", "X") * 480.0f / 2.0f + 240.0f;
    float cY =
        (1.0f - Utilities::Input::get_axis("Mouse", "Y")) * 272.0f / 2.0f;

    if (cX >= 140.0f && cX <= 340.0f) {
        if (cY >= 154 && cY <= 174) {
            selIdx = 0;
        }
        if (cY >= 106 && cY <= 126) {
            selIdx = 1;
        }
        if (cY >= 58 && cY <= 78) {
            selIdx = 2;
        }
    }
#endif
}

} // namespace CrossCraft
