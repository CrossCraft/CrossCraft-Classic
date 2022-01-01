#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

namespace CrossCraft {

void GameState::on_start() { SC_APP_INFO("ENTERING MAIN!"); }
void GameState::on_cleanup() {}

void GameState::on_update(Core::Application *app, double dt) {}
void GameState::on_draw(Core::Application *app, double dt) {}

} // namespace CrossCraft