#include "Gamestate.hpp"
#include "World/DigAction.hpp"
#include "World/Generation/ClassicGenerator.hpp"
#include "World/Generation/CrossCraftGenerator.hpp"
#include "World/PlaceAction.hpp"
#include <Stardust-Celeste.hpp>
#include <Utilities/Controllers/KeyboardController.hpp>
#include <Utilities/Controllers/MouseController.hpp>
#include <Utilities/Controllers/PSPController.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;

GameState::~GameState() { on_cleanup(); }

void GameState::on_start() {

    Rendering::Color clearcol;
    clearcol.color = 0xFFFFD597;

    Rendering::RenderContext::get().set_color(clearcol);

    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());

    // Read config
    world->cfg = Config::loadConfig();

    if (forced_mp) {
#if PSP
        Network::NetworkDriver::get().initGUI();
#endif
        client = create_scopeptr<MP::Client>(world.get(), world->cfg.ip);
        world->client = client.get();
        world->player->client_ref = client.get();
    } else {

        FILE *fptr = fopen("save.ccc", "r");
        if (fptr) {
            if (!world->load_world())
                if (world->cfg.compat)
                    ClassicGenerator::generate(world.get());
                else
                    CrossCraftGenerator::generate(world.get());
            fclose(fptr);
        } else {
            if (world->cfg.compat)
                ClassicGenerator::generate(world.get());
            else
                CrossCraftGenerator::generate(world.get());
        }

        world->spawn();
    }

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
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void GameState::on_update(Core::Application *app, double dt) {
    if (client.get() != nullptr) {
        client->update(dt);

        if (client->is_ready) {
            // Update the user input
            Utilities::Input::update();

            // Update the world
            world->update(dt);
        }
    } else {
        // Update the user input
        Utilities::Input::update();

        // Update the world
        world->update(dt);
    }
}
void GameState::on_draw(Core::Application *app, double dt) {
    if (client.get() != nullptr) {
        client->draw();

        if (client->is_ready) {
            world->draw();
        }
    } else {
        world->draw();
    }
}

/* Ugly Key-Binding Function */

void GameState::bind_controls() {
    //
    // PSP Face Buttons: Release
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::Triangle, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Cross, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Square, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Circle, KeyFlag::Release},
        {Player::move_reset, world->player.get()});

    //
    // PSP Face Buttons: Press/Hold
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::Triangle, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Cross, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Square, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::Circle, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});

    //
    // PSP Directional Buttons: Press/Hold
    //
    psp_controller->add_command({(int)Input::PSPButtons::Up, KeyFlag::Held},
                                {Player::move_up, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Up, KeyFlag::Press},
                                {Player::press_up, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Down, KeyFlag::Press},
                                {Player::press_down, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Left, KeyFlag::Press},
                                {Player::press_left, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Right, KeyFlag::Press},
                                {Player::press_right, world->player.get()});

    //
    // PSP Start/Select: Press
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::Select, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Start, KeyFlag::Press},
                                {World::save, world.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Down, KeyFlag::Press},
                                {Player::psp_chat, world->player.get()});

    //
    // PSP Triggers: Press/Hold
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::RTrigger, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::LTrigger, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    key_controller->add_command({(int)Input::Keys::Escape, KeyFlag::Press},
                                {World::save, world.get()});

    key_controller->add_command({(int)Input::Keys::W, KeyFlag::Release},
                                {Player::move_reset, world->player.get()});
    key_controller->add_command({(int)Input::Keys::S, KeyFlag::Release},
                                {Player::move_reset, world->player.get()});
    key_controller->add_command({(int)Input::Keys::A, KeyFlag::Release},
                                {Player::move_reset, world->player.get()});
    key_controller->add_command({(int)Input::Keys::D, KeyFlag::Release},
                                {Player::move_reset, world->player.get()});

    key_controller->add_command(
        {(int)Input::Keys::W, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::S, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::A, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::D, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});

    key_controller->add_command(
        {(int)Input::Keys::R, KeyFlag::Press | KeyFlag::Held},
        {Player::respawn, RespawnRequest{world->player.get(), world.get()}});

    key_controller->add_command(
        {(int)Input::Keys::Space, KeyFlag::Press | KeyFlag::Held},
        {Player::move_up, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::LShift, KeyFlag::Press | KeyFlag::Held},
        {Player::move_down, world->player.get()});
    key_controller->add_command({(int)Input::Keys::B, KeyFlag::Press},
                                {Player::toggle_inv, world->player.get()});

    key_controller->add_command({(int)Input::Keys::Enter, KeyFlag::Press},
                                {Player::submit_chat, world->player.get()});
    key_controller->add_command({(int)Input::Keys::T, KeyFlag::Press},
                                {Player::enter_chat, world->player.get()});
    key_controller->add_command({(int)Input::Keys::Backspace, KeyFlag::Press},
                                {Player::delete_chat, world->player.get()});

    mouse_controller->add_command(
        {(int)Input::MouseButtons::Left, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    mouse_controller->add_command(
        {(int)Input::MouseButtons::Right, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

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

} // namespace CrossCraft
