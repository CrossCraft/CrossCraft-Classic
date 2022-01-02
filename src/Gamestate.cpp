#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

namespace CrossCraft {

void GameState::on_start() {}
void GameState::on_cleanup() {}

void GameState::on_update(Core::Application *app, double dt) {
    Utilities::Input::update_input();
    player.update(static_cast<float>(dt));
}
void GameState::on_draw(Core::Application *app, double dt) { player.draw(); }

} // namespace CrossCraft