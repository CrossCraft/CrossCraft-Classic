#include "PauseMenu.hpp"
#include "../TexturePackManager.hpp"
#include "../UI/TextHelper.hpp"

#define BUILD_PC (BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX)

#if BUILD_PC
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace GI {
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

    fontRenderer = create_refptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));

    background_rectangle = create_scopeptr<Rendering::Primitive::Rectangle>(
        Rendering::Rectangle{{0, 0}, {480, 272}}, Rendering::Color{0, 0, 0, 96},
        -3);
}
PauseMenu::~PauseMenu() {}

auto PauseMenu::enter() -> void {
#if BUILD_PC
    glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif

    pauseState = 0;
    selIdx = 0;
}
auto PauseMenu::exit() -> void {
#if BUILD_PC
    glfwSetInputMode(GI::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Utilities::Input::set_cursor_center();
#endif

    pauseState = 0;
    selIdx = 0;
}

auto render_with_shadow(RefPtr<Graphics::G2D::FontRenderer> fontRenderer,
                        std::string str, float x, float y) -> void {
    fontRenderer->add_text(str, {x - fontRenderer->calculate_size(str) / 2, y},
                           {255, 255, 255, 255}, -11);
    fontRenderer->add_text(
        str, {(x + 1) - fontRenderer->calculate_size(str) / 2, (y - 1)},
        {63, 63, 63, 255}, -10);
}

auto PauseMenu::draw() -> void {

#if BUILD_PC
    glClear(GL_DEPTH_BUFFER_BIT);
#endif

    background_rectangle->draw();
    fontRenderer->clear_tiles();

    switch (pauseState) {

    case 0: {
        Rendering::RenderContext::get().matrix_translate({0, 10, 0});

        if (selIdx == 0)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 1)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 2)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -48, 0});
        if (selIdx == 3)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        render_with_shadow(fontRenderer, "Game Menu", 240, 216);
        render_with_shadow(fontRenderer, "Back to Game", 240, 160);
        render_with_shadow(fontRenderer, "Options", 240, 136);
        render_with_shadow(fontRenderer, "Save Game", 240, 112);
        render_with_shadow(fontRenderer, "Quit Game", 240, 64);
        break;
    }

    case 1: {
        Rendering::RenderContext::get().matrix_translate({-108, 32, 0});

        if (selIdx == 0)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 1)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 2)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_clear();
        Rendering::RenderContext::get().matrix_translate({108, 32, 0});

        if (selIdx == 3)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 4)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 5)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_clear();
        Rendering::RenderContext::get().matrix_translate({0, -96 + 8, 0});
        if (selIdx == 6)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        Rendering::RenderContext::get().matrix_translate({0, -24, 0});
        if (selIdx == 7)
            sel_sprite->draw();
        else
            unsel_sprite->draw();

        render_with_shadow(fontRenderer, std::string("Option Menu:"), 240, 216);

        render_with_shadow(fontRenderer,
                           std::string("Music: ") + (Option::get().music
                                                         ? std::string("ON")
                                                         : std::string("OFF")),
                           132, 182);

        auto dist = Option::get().renderDist;
        auto str = std::string("");
        switch (dist) {
        case 0: {
            str = "TINY";
            break;
        }
        case 1: {
            str = "SHORT";
            break;
        }
        case 2: {
            str = "NORMAL";
            break;
        }
        case 3: {
            str = "FAR";
            break;
        }
        }

        render_with_shadow(fontRenderer, std::string("Render Distance: ") + str,
                           132, 158);
        render_with_shadow(fontRenderer,
                           std::string("View Bobbing: ") +
                               (Option::get().bobbing ? std::string("ON")
                                                      : std::string("OFF")),
                           132, 134);

        render_with_shadow(fontRenderer,
                           std::string("Sound: ") + (Option::get().sound
                                                         ? std::string("ON")
                                                         : std::string("OFF")),
                           348, 182);
        render_with_shadow(
            fontRenderer,
            std::string("Show FPS: ") +
                (Option::get().fps ? std::string("ON") : std::string("OFF")),
            348, 158);
        render_with_shadow(fontRenderer,
                           std::string("Vsync: ") + (Option::get().vsync
                                                         ? std::string("ON")
                                                         : std::string("OFF")),
                           348, 134);

        render_with_shadow(fontRenderer, "Controls", 240, 62);
        render_with_shadow(fontRenderer, "Back", 240, 62 - 24);

        break;
    }
    }

    Rendering::RenderContext::get().matrix_clear();
    fontRenderer->generate_map();
    fontRenderer->draw();
}
auto PauseMenu::update() -> void {
#if BUILD_PC
    selIdx = -1;
    float cX =
        Utilities::Input::get_axis("Mouse", "X") * 480.0f / 2.0f + 240.0f;
    float cY =
        (1.0f - Utilities::Input::get_axis("Mouse", "Y")) * 272.0f / 2.0f;
    if (pauseState == 0) {
        if (cX >= 140.0f && cX <= 340.0f) {
            if (cY >= 154 && cY <= 174) {
                selIdx = 0;
            }
            if (cY >= 130 && cY <= 154) {
                selIdx = 1;
            }
            if (cY >= 106 && cY <= 126) {
                selIdx = 2;
            }
            if (cY >= 58 && cY <= 78) {
                selIdx = 3;
            }
        }
    }
    else if (pauseState == 1) {
        cY -= 22;
        if (cX >= 32 && cX <= 232) {
            if (cY >= 154 && cY <= 174) {
                selIdx = 0;
            }
            if (cY >= 130 && cY <= 154) {
                selIdx = 1;
            }
            if (cY >= 106 && cY <= 126) {
                selIdx = 2;
            }
        }
        else if (cX >= 248 && cX <= 448) {
            if (cY >= 154 && cY <= 174) {
                selIdx = 3;
            }
            if (cY >= 130 && cY <= 154) {
                selIdx = 4;
            }
            if (cY >= 106 && cY <= 126) {
                selIdx = 5;
            }
        }

        cY += 22;
        if (cX >= 140 && cX <= 340) {
            if (cY >= 56 && cY <= 76) {
                selIdx = 6;
            }
            if (cY >= 36 && cY <= 56) {
                selIdx = 7;
            }
        }
    }
#endif
}

} // namespace CrossCraft
