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
    ~GameState() = default;

    void on_start();
    void on_cleanup();

    void on_update(Core::Application *app, double dt);
    void on_draw(Core::Application *app, double dt);

  private:
    ScopePtr<World> world;
    Utilities::Controller *psp_controller;
    Utilities::Controller *key_controller;
};

} // namespace CrossCraft