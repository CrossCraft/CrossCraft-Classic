#pragma once
#include "../Option.hpp"
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <Utilities/Input.hpp>

namespace CrossCraft {
using namespace Stardust_Celeste;
class PauseMenu {
  public:
    PauseMenu();
    ~PauseMenu();

    auto enter() -> void;
    auto exit() -> void;

    auto draw() -> void;
    auto update() -> void;

    int selIdx = -1;
    int pauseState = 0;

  private:
    uint32_t gui_tex;
    uint32_t font_texture;

    ScopePtr<Graphics::G2D::Sprite> unsel_sprite;
    ScopePtr<Graphics::G2D::Sprite> sel_sprite;
    ScopePtr<Graphics::G2D::Sprite> dis_sprite;
    RefPtr<Graphics::G2D::FontRenderer> fontRenderer;

    ScopePtr<Rendering::Primitive::Rectangle> background_rectangle;
};
} // namespace CrossCraft