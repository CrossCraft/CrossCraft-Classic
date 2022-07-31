#include "TextHelper.hpp"
#include "../TexturePackManager.hpp"

namespace CrossCraft {

TextHelper::TextHelper() {
    font_texture = TexturePackManager::get().load_texture(
        "assets/default.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        false, false);

    fontRenderer = create_scopeptr<Graphics::G2D::FontRenderer>(
        font_texture, glm::vec2(16, 16));
}

auto TextHelper::clear() -> void { fontRenderer->clear(); }
auto TextHelper::draw() -> void {
    fontRenderer->rebuild();

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
#endif // PSP

#if BUILD_PLAT == BUILD_VITA
    glDisable(GL_DEPTH_TEST);
#endif
    fontRenderer->draw();

#if BUILD_PLAT == BUILD_VITA
    glEnable(GL_DEPTH_TEST);
#endif
}

auto TextHelper::get_width(std::string text) -> float {
    return fontRenderer->calculate_size(text);
}

auto TextHelper::draw_text(std::string text, glm::vec2 pos, unsigned char col,
                           u8 alpha, short bg_mode) -> void {
    // Generate RGB color values
    Rendering::Color front;
    Rendering::Color back;

    // Fill the values
    switch (col) {
    case CC_TEXT_COLOR_BLACK:
        front = {0, 0, 0, alpha};
        back = {0, 0, 0, alpha};
        break;
    case CC_TEXT_COLOR_DARK_BLUE:
        front = {0, 0, 170, alpha};
        back = {0, 0, 42, alpha};
        break;
    case CC_TEXT_COLOR_DARK_GREEN:
        front = {0, 170, 0, alpha};
        back = {0, 42, 0, alpha};
        break;
    case CC_TEXT_COLOR_DARK_AQUA:
        front = {0, 170, 170, alpha};
        back = {0, 42, 42, alpha};
        break;
    case CC_TEXT_COLOR_DARK_RED:
        front = {170, 0, 0, alpha};
        back = {42, 0, 0, alpha};
        break;
    case CC_TEXT_COLOR_DARK_PURPLE:
        front = {170, 0, 170, alpha};
        back = {42, 0, 42, alpha};
        break;
    case CC_TEXT_COLOR_GOLD:
        front = {255, 170, 0, alpha};
        back = {42, 42, 0, alpha};
        break;
    case CC_TEXT_COLOR_GRAY:
        front = {170, 170, 170, alpha};
        back = {42, 42, 42, alpha};
        break;
    case CC_TEXT_COLOR_DARK_GRAY:
        front = {85, 85, 85, alpha};
        back = {21, 21, 21, alpha};
        break;
    case CC_TEXT_COLOR_BLUE:
        front = {85, 85, 255, alpha};
        back = {21, 21, 63, alpha};
        break;
    case CC_TEXT_COLOR_GREEN:
        front = {85, 255, 85, alpha};
        back = {21, 63, 21, alpha};
        break;
    case CC_TEXT_COLOR_AQUA:
        front = {85, 255, 255, alpha};
        back = {21, 63, 63, alpha};
        break;
    case CC_TEXT_COLOR_RED:
        front = {255, 85, 85, alpha};
        back = {21, 63, 63, alpha};
        break;
    case CC_TEXT_COLOR_LIGHT_PURPLE:
        front = {255, 85, 255, alpha};
        back = {63, 21, 63, alpha};
        break;
    case CC_TEXT_COLOR_YELLOW:
        front = {255, 255, 85, alpha};
        back = {63, 63, 21, alpha};
        break;
    default:
    case CC_TEXT_COLOR_WHITE:
        front = {255, 255, 255, alpha};
        back = {63, 63, 63, alpha};
        break;
    case CC_TEXT_COLOR_BE_MTX_GOLD:
        front = {221, 214, 5, alpha};
        back = {55, 53, 1, alpha};
        break;
    }

    //if (bg_mode == CC_TEXT_BG_DYNAMIC) {
    //    Rendering::RenderContext::get().draw_rect(pos + glm::vec2(-1, -1),
    //                                              {112, 12}, {0, 0, 0, 128}, 2);
    //}

    fontRenderer->add_text(text, {pos.x + 1, pos.y - 1}, back, 1.0f);
    fontRenderer->add_text(text, {pos.x, pos.y}, front, 0.0f);
}

} // namespace CrossCraft
