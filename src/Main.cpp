#include "Menu/Menustate.hpp"
#include <Stardust-Celeste.hpp>

#ifdef _WIN32
#include <windows.h>
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance =
    0x00000001;
#endif

using namespace Stardust_Celeste;

/**
 * @brief Game Application
 *
 */
class GameApplication : public Core::Application {
  public:
    /**
     * @brief On Start override
     */
    void on_start() override {

        // Create new Game State
        auto state = create_refptr<CrossCraft::MenuState>();
        // Set to our state
        this->set_state(state);

        // Set the background color
        Rendering::Color clearcol;
        clearcol.color = 0xFFFFFFFF;

        Rendering::RenderContext::get().set_color(clearcol);
    }
};

/**
 * @brief Create a New Stardust Celeste Application object
 *
 * @return Core::Application*
 */
Core::Application *CreateNewSCApp() {

    // Configure the engine
    Core::AppConfig config;
    config.headless = false;
    config.networking = true;
    config.render_settings.title = "CrossCraft-Classic";
    config.render_settings.width = 960;
    config.render_settings.height = 544;

    Core::PlatformLayer::get().initialize(config);

    // Return the game
    return new GameApplication();
}
