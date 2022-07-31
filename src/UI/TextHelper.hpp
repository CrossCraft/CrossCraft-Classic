#pragma once

#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Rendering.hpp>
#include <Utilities/Types.hpp>

#include <Rendering/Primitive/Rectangle.hpp>

// creds to https://minecraft.fandom.com/wiki/Formatting_codes
#define CC_TEXT_COLOR_BLACK 0
#define CC_TEXT_COLOR_DARK_BLUE 1
#define CC_TEXT_COLOR_DARK_GREEN 2
#define CC_TEXT_COLOR_DARK_AQUA 3
#define CC_TEXT_COLOR_DARK_RED 4
#define CC_TEXT_COLOR_DARK_PURPLE 5
#define CC_TEXT_COLOR_GOLD 6
#define CC_TEXT_COLOR_GRAY 7
#define CC_TEXT_COLOR_DARK_GRAY 8
#define CC_TEXT_COLOR_BLUE 9
#define CC_TEXT_COLOR_GREEN 10
#define CC_TEXT_COLOR_AQUA 11
#define CC_TEXT_COLOR_RED 12
#define CC_TEXT_COLOR_LIGHT_PURPLE 13
#define CC_TEXT_COLOR_YELLOW 14
#define CC_TEXT_COLOR_WHITE 15
#define CC_TEXT_COLOR_BE_MTX_GOLD 16

// For fixed, just specify length..
#define CC_TEXT_BG_NONE 0
#define CC_TEXT_BG_DYNAMIC 1

using namespace Stardust_Celeste;

namespace CrossCraft {

class TextHelper {

  public:
    /**
     * @brief Construct a new TextHelper object
     *
     */
    TextHelper();

    /**
     * @brief Destroy the TextHelper object
     *
     */
    ~TextHelper() = default;

    /*
     * @breif Wrapper for FontRenderer::calculate_size
     *
     * @param text Text to get the pixel size of
     */
    auto get_width(std::string text) -> float;

    /**
     * @brief Enhanced wrapper for FontRenderer
     *
     * @param text Text to draw.
     * @param pos X, Y coordinates of element, relative to 2D buffer.
     * @param col Color definition for the text (0-16).
     * @param alpha Opacity in alpha8 for the text element.
     * @param draw_bg Draw a rectangle under the text to improve legibility.
     */
    auto draw_text(std::string text, glm::vec2 pos, unsigned char col, u8 alpha,
                   short bg_mode) -> void;
    auto clear() -> void;
    auto draw() -> void;

  private:
    uint32_t font_texture;
    ScopePtr<Graphics::G2D::FontRenderer> fontRenderer;

    ScopePtr<Rendering::Primitive::Rectangle> background_rectangle;

}; // class TextHelper

} // namespace CrossCraft
