#include "Gamestate.hpp"
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/PSPController.hpp>

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;

void GameState::bind_controls() {
    psp_controller->add_command(
        {(int)Input::PSPButtons::Triangle, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Square, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Circle, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Cross, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Select, KeyFlag::Press | KeyFlag::Held},
        {Player::move_down, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Up, KeyFlag::Press | KeyFlag::Held},
        {Player::move_up, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::LTrigger, KeyFlag::Press},
        {World::dig, world.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::RTrigger, KeyFlag::Press},
        {World::place, world.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Left, KeyFlag::Press},
                                {Player::dec_selector, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Right, KeyFlag::Press},
                                {Player::inc_selector, world->player.get()});

    key_controller->add_command(
        {(int)Input::Keys::W, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::A, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::D, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::S, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::Space, KeyFlag::Press | KeyFlag::Held},
        {Player::move_up, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::LShift, KeyFlag::Press | KeyFlag::Held},
        {Player::move_down, world->player.get()});
    mouse_controller->add_command(
        {(int)Input::MouseButtons::Left, KeyFlag::Press},
        {World::dig, world.get()});
    mouse_controller->add_command(
        {(int)Input::MouseButtons::Right, KeyFlag::Press},
        {World::place, world.get()});

    key_controller->add_command(
        {(int)Input::Keys::Num1, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 0}});
    key_controller->add_command(
        {(int)Input::Keys::Num2, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 1}});
    key_controller->add_command(
        {(int)Input::Keys::Num3, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 2}});
    key_controller->add_command(
        {(int)Input::Keys::Num4, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 3}});
    key_controller->add_command(
        {(int)Input::Keys::Num5, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 4}});
    key_controller->add_command(
        {(int)Input::Keys::Num6, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 5}});
    key_controller->add_command(
        {(int)Input::Keys::Num7, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 6}});
    key_controller->add_command(
        {(int)Input::Keys::Num8, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 7}});
    key_controller->add_command(
        {(int)Input::Keys::Num9, KeyFlag::Press | KeyFlag::Held},
        {Player::change_selector, SelData{world->player.get(), 8}});

    Input::add_controller(psp_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);

    Input::set_differential_mode("Mouse", true);
    Input::set_differential_mode("PSP", true);
}

GameState::~GameState() { on_cleanup(); }

void GameState::on_start() {
    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());
    world->generate();

    // Make new controllers
    psp_controller = new Input::PSPController();
    key_controller = new Input::KeyboardController();
    mouse_controller = new Input::MouseController();

    // Bind our controllers
    bind_controls();

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();
}

void GameState::on_cleanup() {
    delete psp_controller;
    delete key_controller;
    delete mouse_controller;
}

void GameState::quit(std::any d) {
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void GameState::on_update(Core::Application *app, double dt) {

    // Setup quit command
    if (!ref) {
        ref = app;

        psp_controller->add_command(
            {(int)Input::PSPButtons::Start, KeyFlag::Press}, {quit, ref});
        key_controller->add_command({(int)Input::Keys::Escape, KeyFlag::Press},
                                    {quit, ref});
    }

    // Update the world
    world->update(dt);

    // Update the user input
    Utilities::Input::update();
}
void GameState::on_draw(Core::Application *app, double dt) { world->draw(); }

} // namespace CrossCraft