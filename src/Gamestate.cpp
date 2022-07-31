#include "Gamestate.hpp"

#include "TexturePackManager.hpp"
#include "Utils.hpp"
#include "World/Generation/ClassicGenerator.hpp"
#include "World/Generation/CrossCraftGenerator.hpp"

namespace CrossCraft {
using namespace Stardust_Celeste::Utilities;

GameState::~GameState() { on_cleanup(); }

void GameState::on_start() {
    // Set Color
    Rendering::RenderContext::get().set_color(
        Rendering::Color{0x99, 0xCC, 0xFF, 0xFF});

    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());

    // Read config
    world->cfg = Config::loadConfig();

    if (forced_mp) {
        // Connect to Multiplayer
        client = create_scopeptr<MP::Client>(world.get(), world->cfg.ip);
        world->client = client.get();
        world->player->client_ref = client.get();
    } else {
        // Try Load Save -- if fails, do generation

        FILE *fptr = fopen((PLATFORM_FILE_PREFIX + "save.ccc").c_str(), "r");
        if (!fptr || !world->load_world()) {
            if (world->cfg.compat)
                ClassicGenerator::generate(world.get());
            else
                CrossCraftGenerator::generate(world.get());
        }
        if(fptr != nullptr)
            fclose(fptr);

        world->spawn();
    }

    // Make new controllers
    psp_controller = new Input::PSPController();
    vita_controller = new Input::VitaController();
    key_controller = new Input::KeyboardController();
    mouse_controller = new Input::MouseController();

    // Bind our controllers
    bind_controls();

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();
}

void GameState::on_cleanup() {
    delete psp_controller;
    delete vita_controller;
    delete key_controller;
    delete mouse_controller;
}

void GameState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void GameState::on_update(Core::Application *app, double dt) {
    if (client.get() != nullptr)
        client->update(dt);

    if (client.get() == nullptr || client->is_ready) {
        Input::update();
        world->update(dt);
    }
}
void GameState::on_draw(Core::Application *app, double dt) {
    if (client.get() != nullptr)
        client->draw();

    if (client.get() == nullptr || client->is_ready)
        world->draw();
}
} // namespace CrossCraft
