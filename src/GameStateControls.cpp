#include "Gamestate.hpp"

#include "Controls.hpp"
#include "World/DigAction.hpp"
#include "World/PlaceAction.hpp"
#include "World/SaveData.hpp"
namespace CrossCraft {

using namespace Stardust_Celeste::Utilities;
/* Ugly Key-Binding Function */

void GameState::bind_controls() {

    psp_controller->clear_command();
    key_controller->clear_command();
    mouse_controller->clear_command();
    vita_controller->clear_command();
    n3ds_controller->clear_command();

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

    n3ds_controller->add_command({(int)Input::N3DSButtons::Y, KeyFlag::Release},
                                 {Player::move_reset, world->player.get()});
    n3ds_controller->add_command({(int)Input::N3DSButtons::A, KeyFlag::Release},
                                 {Player::move_reset, world->player.get()});
    n3ds_controller->add_command({(int)Input::N3DSButtons::X, KeyFlag::Release},
                                 {Player::move_reset, world->player.get()});
    n3ds_controller->add_command({(int)Input::N3DSButtons::B, KeyFlag::Release},
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

    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Y, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::A, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::X, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::B, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});

    vita_controller->add_command(
        {(int)Input::VitaButtons::Cross, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});

    //
    // PSP Directional Buttons: Press/Hold
    //
    psp_controller->add_command(
        {(int)Controls::get().buttonJump, KeyFlag::Held},
        {Player::move_up, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Up, KeyFlag::Press},
                                {Player::press_up, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Down, KeyFlag::Press},
                                {Player::press_down, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Left, KeyFlag::Press},
                                {Player::press_left, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Right, KeyFlag::Press},
                                {Player::press_right, world->player.get()});

    n3ds_controller->add_command(
        {(int)Controls::get().buttonJump, KeyFlag::Held},
        {Player::move_up, world->player.get()});
    n3ds_controller->add_command({(int)Input::N3DSButtons::Dup, KeyFlag::Press},
                                 {Player::press_up, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Ddown, KeyFlag::Press},
        {Player::press_down, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Dleft, KeyFlag::Press},
        {Player::press_left, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Dright, KeyFlag::Press},
        {Player::press_right, world->player.get()});

    vita_controller->add_command(
        {(int)Controls::get().buttonJump, KeyFlag::Held},
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
        {(int)Controls::get().buttonMenu, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    psp_controller->add_command({(int)Input::PSPButtons::Start, KeyFlag::Press},
                                {Player::pause, world->player.get()});

    n3ds_controller->add_command(
        {(int)Controls::get().buttonMenu, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    n3ds_controller->add_command(
        {(int)Input::N3DSButtons::Start, KeyFlag::Press},
        {Player::pause, world->player.get()});

    vita_controller->add_command(
        {(int)Controls::get().buttonMenu, KeyFlag::Press},
        {Player::toggle_inv, world->player.get()});
    vita_controller->add_command(
        {(int)Input::VitaButtons::Start, KeyFlag::Press},
        {Player::pause, world->player.get()});

    //
    // PSP Triggers: Press/Hold
    //
    psp_controller->add_command(
        {(int)Controls::get().buttonBreak, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    psp_controller->add_command(
        {(int)Controls::get().buttonPlace, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    n3ds_controller->add_command(
        {(int)Controls::get().buttonBreak, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    n3ds_controller->add_command(
        {(int)Controls::get().buttonPlace, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    vita_controller->add_command(
        {(int)Controls::get().buttonBreak, KeyFlag::Press | KeyFlag::Held},
        {DigAction::dig, world.get()});
    vita_controller->add_command(
        {(int)Controls::get().buttonPlace, KeyFlag::Press | KeyFlag::Held},
        {PlaceAction::place, world.get()});

    key_controller->add_command({(int)Input::Keys::Escape, KeyFlag::Press},
                                {Player::pause, world->player.get()});

    key_controller->add_command(
        {(int)Controls::get().keyForward, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyBack, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyLeft, KeyFlag::Release},
        {Player::move_reset, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyRight, KeyFlag::Release},
        {Player::move_reset, world->player.get()});

    key_controller->add_command(
        {(int)Controls::get().keyForward, KeyFlag::Press | KeyFlag::Held},
        {Player::move_forward, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyBack, KeyFlag::Press | KeyFlag::Held},
        {Player::move_backward, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyLeft, KeyFlag::Press | KeyFlag::Held},
        {Player::move_left, world->player.get()});
    key_controller->add_command(
        {(int)Controls::get().keyRight, KeyFlag::Press | KeyFlag::Held},
        {Player::move_right, world->player.get()});

    key_controller->add_command(
        {(int)Controls::get().keyRespawn, KeyFlag::Press | KeyFlag::Held},
        {Player::respawn, RespawnRequest{world->player.get(), world.get()}});

    key_controller->add_command(
        {(int)Controls::get().keyJump, KeyFlag::Press | KeyFlag::Held},
        {Player::move_up, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::LShift, KeyFlag::Press | KeyFlag::Held},
        {Player::move_down, world->player.get()});
    key_controller->add_command({(int)Input::Keys::B, KeyFlag::Press},
                                {Player::toggle_inv, world->player.get()});

    key_controller->add_command({(int)Input::Keys::Enter, KeyFlag::Press},
                                {Player::submit_chat, world->player.get()});
    key_controller->add_command({(int)Controls::get().keyChat, KeyFlag::Press},
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

    // Mouse wheel to cycle through objects
    mouse_controller->add_command(
        {(int)Input::MouseButtons::MWheelUp, KeyFlag::Press},
        {Player::press_left, world->player.get()});
    mouse_controller->add_command(
        {(int)Input::MouseButtons::MWheelDown, KeyFlag::Press | KeyFlag::Held},
        {Player::press_right, world->player.get()});

    // Map directions to numpad
    key_controller->add_command({(int)Input::Keys::KeyPad8, KeyFlag::Press},
                                {Player::press_up, world->player.get()});
    key_controller->add_command({(int)Input::Keys::KeyPad2, KeyFlag::Press},
                                {Player::press_down, world->player.get()});
    key_controller->add_command({(int)Input::Keys::KeyPad4, KeyFlag::Press},
                                {Player::press_left, world->player.get()});
    key_controller->add_command({(int)Input::Keys::KeyPad6, KeyFlag::Press},
                                {Player::press_right, world->player.get()});

    key_controller->add_command({(int)Controls::get().keyTab, KeyFlag::Release},
                                {Player::tab_end, world->player.get()});
    key_controller->add_command(
        {(int)Input::Keys::Tab, KeyFlag::Press | KeyFlag::Held},
        {Player::tab_start, world->player.get()});
}

} // namespace CrossCraft
