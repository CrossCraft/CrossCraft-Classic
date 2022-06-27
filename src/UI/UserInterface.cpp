#include "UserInterface.hpp"
#include "../BlockConst.hpp"

namespace CrossCraft {

UserInterface::UserInterface() {
    textHelper = create_scopeptr<TextHelper>();
}

auto UserInterface::get_block_name(uint8_t id) -> std::string {

    // Stupid wool hack
    if (id >= 21 && id <= 36)
        return "Wool";

    switch(id) {
        case Block::Air: return "Air"; break;
        case Block::Stone: return "Stone"; break;
        case Block::Dirt: return "Dirt"; break;
        case Block::Grass: return "Grass Block"; break;
        case Block::Cobblestone: return "Cobblestone"; break;
        case Block::Wood: return "Wood"; break;
        case Block::Sapling: return "Sapling"; break;
        case Block::Bedrock: return "Bedrock"; break;
        case Block::Water: case Block::Still_Water: return "Water"; break;
        case Block::Lava: case Block::Still_Lava: return "Lava"; break;
        case Block::Sand: return "Sand"; break;
        case Block::Gravel: return "Gravel"; break;
        case Block::Gold_Ore: return "Gold Ore"; break;
        case Block::Iron_Ore: return "Iron Ore"; break;
        case Block::Coal_Ore: return "Coal Ore"; break;
        case Block::Logs: return "Wood Log"; break;
        case Block::Leaves: return "Leaves"; break;
        case Block::Sponge: return "Sponge"; break;
        case Block::Glass: return "Glass"; break;
        case Block::Flower1: case Block::Flower2: return "Flower"; break;
        case Block::Mushroom1: case Block::Mushroom2: return "Mushroom"; break;
        case Block::Gold: return "Gold Block"; break;
        case Block::Iron: return "Iron Block"; break;
        case Block::Slab: return "Stone Slab"; break;
        case Block::Brick: return "Bricks"; break;
        case Block::TNT: return "TNT"; break;
        case Block::Bookshelf: return "Bookshelf"; break;
        case Block::Mossy_Rocks: return "Mossy Cobblestone"; break;
        case Block::Obsidian: return "Obsidian"; break;
        default: return "Unknown Block Name"; break;
    }
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
        case CC_TEXT_ALIGN_CENTER: x_position = 240 -
            static_cast<short>(textHelper->get_width(text)/2); break;
        case CC_TEXT_ALIGN_RIGHT: x_position = 478 -
            static_cast<short>(textHelper->get_width(text)); break;
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
