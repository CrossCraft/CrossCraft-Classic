#include "UserInterface.hpp"

namespace CrossCraft {

UserInterface::UserInterface() {
    textHelper = create_scopeptr<TextHelper>();
}


auto UserInterface::begin2D() -> void {
    Rendering::RenderContext::get().set_mode_2D();
    Rendering::RenderContext::get().matrix_ortho(0, 480, 0, 272, 100, -100);

    textHelper->clear();
}

auto UserInterface::end2D() -> void {
    textHelper->draw();
}

auto UserInterface::draw_text(std::string text, unsigned char color,
                              unsigned char x_align, unsigned char y_align,
                              short x_line, short y_line, short bg_mode)
-> void {

    short x_position = 0;
    short y_position = 0;

    switch(x_align) {
        case CC_TEXT_ALIGN_LEFT: x_position = 2; break;
        case CC_TEXT_ALIGN_CENTER: x_position = 240 - static_cast<short>(textHelper->get_width(text)/2); break;
        case CC_TEXT_ALIGN_RIGHT: x_position = 478 - static_cast<short>(textHelper->get_width(text)); break;
        default: break;
    }

    switch(y_align) {
        case CC_TEXT_ALIGN_TOP: y_position = 262; break;
        case CC_TEXT_ALIGN_CENTER: y_position = 132; break;
        case CC_TEXT_ALIGN_BOTTOM: y_position = 2; break;
        default: break;
    }

    if (x_line != 0)
        x_position += (5 * x_line);

    if (y_line != 0)
        y_position += (10 * y_line);

    textHelper->draw_text(text, {x_position, y_position}, color,
                          255, false);
}

} // namespace CrossCraft
