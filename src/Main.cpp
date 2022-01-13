#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

using namespace Stardust_Celeste;

/**
 * @brief Class to run the game state
 *
 */
class GameApplication : public Core::Application {
  public:
    /**
     * @brief On Start override
     */
    void on_start() override {

        // Create new Game State
        state = create_refptr<CrossCraft::GameState>();
        // Set to our state
        this->set_state(state);

        // Set the background color
        Rendering::Color clearcol;
        clearcol.color = 0xFFFFD597;

        Rendering::RenderContext::get().set_color(clearcol);
    }

  private:
    RefPtr<CrossCraft::GameState> state;
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
    config.render_settings.title = "CrossCraft-Classic";

    Core::PlatformLayer::get().initialize(config);

    // Return the game
    return new GameApplication();
}