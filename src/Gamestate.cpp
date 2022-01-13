#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

namespace CrossCraft {

void GameState::on_start() {
    world = create_scopeptr<World>(create_refptr<Player>());
    world->generate();
}
void GameState::on_cleanup() {}

void GameState::on_update(Core::Application *app, double dt) {
    Utilities::Input::update_input();
    world->update(dt);
}
void GameState::on_draw(Core::Application *app, double dt) { world->draw(); }

} // namespace CrossCraft