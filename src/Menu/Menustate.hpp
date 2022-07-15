#include "../Config.hpp"
#include "../TexturePackManager.hpp"
#include <Core/Application.hpp>
#include <Graphics/2D/FontRenderer.hpp>
#include <Graphics/2D/Sprite.hpp>
#include <Stardust-Celeste.hpp>
#include <Utilities/Input.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {
using namespace Stardust_Celeste;
/**
 * @brief Game State Class
 *
 */
class MenuState : public Core::ApplicationState {
  public:
    MenuState()
        : key_controller(nullptr), psp_controller(nullptr),
          mouse_controller(nullptr){};
    ~MenuState();

    void on_start();
    void on_cleanup();

    void bind_controls();

    void on_update(Core::Application *app, double dt);
    void on_draw(Core::Application *app, double dt);

    static void quit(std::any d);

  private:
    Utilities::Controller *psp_controller;
    Utilities::Controller *key_controller;
    Utilities::Controller *mouse_controller;

    uint32_t bg_texture;
    ScopePtr<Graphics::G2D::Sprite> bg_tile;

    uint32_t logo_texture;
    ScopePtr<Graphics::G2D::Sprite> logo_sprite;

    uint32_t gui_tex;

    ScopePtr<Graphics::G2D::Sprite> unsel_sprite;
    ScopePtr<Graphics::G2D::Sprite> sel_sprite;

    uint32_t font_texture;
    ScopePtr<Graphics::G2D::FontRenderer> fontRenderer;
    ScopePtr<Graphics::G2D::FontRenderer> splashRenderer;

    float scaleFactor;
    float scaleTimer;
};

} // namespace CrossCraft