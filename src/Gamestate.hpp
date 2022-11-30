#include <Core/Application.hpp>
#include <Stardust-Celeste.hpp>
#include <Utilities/Input.hpp>

#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/N3DSController.hpp>
#include <Utilities/Controllers/PSPController.hpp>
#include <Utilities/Controllers/VitaController.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "Config.hpp"
#include "MP/Client.hpp"
#include "World/World.hpp"

using namespace Stardust_Celeste;

namespace CrossCraft {

/**
 * @brief Main Game State
 *
 */
class GameState : public Core::ApplicationState {
  public:
    /**
     * @brief Construct a new Game State object
     *
     * @param forceMP Forced Multiplayer (MP Connect)
     */
    GameState(bool forceMP = false)
        : key_controller(nullptr), psp_controller(nullptr),
          mouse_controller(nullptr), forced_mp(forceMP){};

    /**
     * @brief Destroy the Game State object
     *
     */
    ~GameState();

    static void apply_controls();

    /**
     * @brief On Start initialization
     *
     */
    void on_start();

    /**
     * @brief On Cleanup deinitialization
     *
     */
    void on_cleanup();

    /**
     * @brief Bind Controls
     *
     */
    void bind_controls();

    /**
     * @brief On Update Events
     *
     * @param app Reference to our Application
     * @param dt Delta Time
     */
    void on_update(Core::Application *app, double dt);
    void on_draw(Core::Application *app, double dt);

  private:
    /**
     * @brief Quit Command to Bind
     *
     * @param d
     */
    static void quit(std::any d);

    /**
     * @brief Multiplayer Client if Needed
     *
     */
    ScopePtr<MP::Client> client;

    /**
     * @brief Game World
     *
     */
    ScopePtr<World> world;

    bool forced_mp;

    /**
     * @brief Game Controllers
     *
     */
    Utilities::Controller *psp_controller;
    Utilities::Controller *vita_controller;
    Utilities::Controller *key_controller;
    Utilities::Controller *mouse_controller;
    Utilities::Controller *n3ds_controller;
};

} // namespace CrossCraft