#include "Gamestate.hpp"

#include "World/DigAction.hpp"
#include "World/PlaceAction.hpp"
#include "World/SaveData.hpp"

namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;
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

    vita_controller->add_command(
        {(int)Input::VitaButtons::Triangle, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Cross, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Square, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Circle, KeyFlag::Release},
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

    vita_controller->add_command(
        {(int)Input::VitaButtons::Triangle, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Cross, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Square, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Circle, KeyFlag::Press | KeyFlag::Held},
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

    vita_controller->add_command({(int)Input::VitaButtons::Up, KeyFlag::Held},
                                 {Player::move_up, world->player.get()});
    vita_controller->add_command({(int)Input::VitaButtons::Up, KeyFlag::Press},
                                 {Player::press_up, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Down, KeyFlag::Press},
        {Player::press_down, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Left, KeyFlag::Press},
        {Player::press_left, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Right, KeyFlag::Press},
        {Player::press_right, world->player.get()});

    //
    // PSP Start/Select: Press
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::Select, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Start, KeyFlag::Press},
                                {SaveData::save, world.get()});

    vita_controller->add_command(
        {(int)Input::VitaButtons::Select, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Start, KeyFlag::Press},
        {SaveData::save, world.get()});

    //
    // PSP Triggers: Press/Hold
    //
    psp_controller->add_command(
        {(int)Input::PSPButtons::RTrigger, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    psp_controller->add_command(
        {(int)Input::PSPButtons::LTrigger, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    vita_controller->add_command(
        {(int)Input::VitaButtons::RTrigger, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::LTrigger, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    key_controller->add_command({(int)Input::Keys::Escape, KeyFlag::Press},
                                {SaveData::save, world.get()});

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
    key_controller->add_command(
        {(int)Input::Keys::Slash, KeyFlag::Press},
        {Player::enter_chat_slash, world->player.get()});
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


    key_controller->add_command({ (int)Input::Keys::Tab, KeyFlag::Release },
        { Player::tab_end, world->player.get() });
    key_controller->add_command({ (int)Input::Keys::Tab, KeyFlag::Press | KeyFlag::Held },
        { Player::tab_start, world->player.get() });

    Input::add_controller(psp_controller);
    Input::add_controller(key_controller);
    Input::add_controller(mouse_controller);
    Input::add_controller(vita_controller);

    Input::set_differential_mode("Mouse", true);
    Input::set_differential_mode("PSP", true);
    Input::set_differential_mode("Vita", true);
}

} // namespace CrossCraft
