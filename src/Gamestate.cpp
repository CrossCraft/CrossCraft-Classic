#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/PSPController.hpp>

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;

void GameState::on_start() {
    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());
    world->generate();

    psp_controller = new Input::PSPController();
    key_controller = new Input::KeyboardController();

    psp_controller->add_command((int)Input::PSPButtons::Triangle,
                                {Player::move_forward, world->player.get()});
    psp_controller->add_command((int)Input::PSPButtons::Square,
                                {Player::move_left, world->player.get()});
    psp_controller->add_command((int)Input::PSPButtons::Circle,
                                {Player::move_right, world->player.get()});
    psp_controller->add_command((int)Input::PSPButtons::Cross,
                                {Player::move_backward, world->player.get()});

    key_controller->add_command((int)Input::Keys::W,
                                {Player::move_forward, world->player.get()});
    key_controller->add_command((int)Input::Keys::A,
                                {Player::move_left, world->player.get()});
    key_controller->add_command((int)Input::Keys::D,
                                {Player::move_right, world->player.get()});
    key_controller->add_command((int)Input::Keys::S,
                                {Player::move_backward, world->player.get()});

    Input::add_controller(psp_controller);
    Input::add_controller(key_controller);

    Input::set_differential_mode("Mouse", true);
    Input::set_differential_mode("PSP", true);
}
void GameState::on_cleanup() {}

void GameState::on_update(Core::Application *app, double dt) {

    // Update the world
    world->update(dt);

    // Update the user input
    Utilities::Input::update();
}
void GameState::on_draw(Core::Application *app, double dt) { world->draw(); }

} // namespace CrossCraft