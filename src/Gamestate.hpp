/**
 * @file Gamestate.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Current Game State
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Config.hpp"
#include "World.hpp"
#include <Core/Application.hpp>
#include <Utilities/Input.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

/**
 * @brief Game State Class
 *
 */
class GameState : public Core::ApplicationState {
  public:
    GameState() = default;
    ~GameState();

    void on_start();
    void on_cleanup();

    void bind_controls();

    void on_update(Core::Application *app, double dt);
    void on_draw(Core::Application *app, double dt);

    static void quit(std::any d);

  private:
    Core::Application *ref;
    ScopePtr<World> world;
    Utilities::Controller *psp_controller;
    Utilities::Controller *key_controller;
    Utilities::Controller *mouse_controller;
    Config config;
};

} // namespace CrossCraft