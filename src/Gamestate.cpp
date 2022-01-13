#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>

namespace CrossCraft {

void GameState::on_start() {
    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());
    world->generate();
}
void GameState::on_cleanup() {}

void GameState::on_update(Core::Application *app, double dt) {
    // Update the user input
    Utilities::Input::update_input();

    // Update the world
    world->update(dt);
}
void GameState::on_draw(Core::Application *app, double dt) { world->draw(); }

} // namespace CrossCraft