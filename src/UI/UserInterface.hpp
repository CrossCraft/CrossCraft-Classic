#pragma once

#include "TextHelper.hpp"

// Text Alignment
#define CC_TEXT_ALIGN_TOP       0
#define CC_TEXT_ALIGN_BOTTOM    1
#define CC_TEXT_ALIGN_LEFT      0
#define CC_TEXT_ALIGN_RIGHT     1
#define CC_TEXT_ALIGN_CENTER    2

using namespace Stardust_Celeste;

namespace CrossCraft {

class UserInterface {

  public:
    /**
     * @brief Construct a new UserInterface object
     *
     */
    UserInterface();

    /**
     * @brief Destroy the UserInterface object
     *
     */
    ~UserInterface() = default;

    auto draw_text(std::string text, unsigned char color, unsigned char x_align,
                   unsigned char y_align, short x_line, short y_line,
                   short bg_mode) -> void;

    auto get_block_name(uint8_t id) -> std::string;

    auto begin2D() -> void;
    auto end2D() -> void;

  private:
    ScopePtr<TextHelper> textHelper;

}; // class UserInterface

} // namespace CrossCraft

