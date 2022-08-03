#include "Gamestate.hpp"

#include "TexturePackManager.hpp"
#include "Utils.hpp"
#include "World/Generation/ClassicGenerator.hpp"
#include "World/Generation/CrossCraftGenerator.hpp"
#include "World/SaveData.hpp"
#include "Rendering/ShaderManager.hpp"

namespace CrossCraft {
using namespace Stardust_Celeste::Utilities;

GameState::~GameState() { on_cleanup(); }


#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
const std::string vert_source = R"(
    #version 400
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aCol;
    layout (location = 2) in vec2 aTex;

    layout (std140) uniform Matrices {
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;
    };
    
    out vec2 uv;
    out vec4 color;
    out vec3 position;

    void main() {
        gl_Position = proj * view * model * vec4(aPos, 1.0);
        position = gl_Position.xyz;
        uv = aTex;
        color = aCol;
    }
)";

const std::string frag_source = R"(
    #version 400
    uniform sampler2D tex;
    in vec2 uv;
    in vec4 color;
    in vec3 position;

    out vec4 FragColor;

    const vec3 fogColor = vec3(0.59765f, 0.796875, 1.0f);
    const float density = 0.0005f;

    void main() {
        vec4 texColor = texture(tex, uv);
        texColor *= vec4(1.0f / 255.0f) * color;

        float dist = abs(position.z);
        const float fogMax = (8.0f * 16.0f * 0.8);
        const float fogMin = (8.0f * 16.0f * 0.2);
        float fogFactor = (fogMax - dist) / (fogMax - fogMin);
        fogFactor = clamp(fogFactor, 0.0f, 1.0f);

        FragColor = vec4(mix(fogColor.rgb, texColor.rgb, fogFactor), texColor.a); 

        if(FragColor.a < 0.1f)
            discard;
   }
)";
#endif

void GameState::on_start() {
    // Set Color
    Rendering::RenderContext::get().set_color(
        Rendering::Color{0x99, 0xCC, 0xFF, 0xFF});

#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX
    auto shad = Rendering::ShaderManager::get().load_shader(vert_source, frag_source);
    Rendering::ShaderManager::get().bind_shader(shad);
#endif;

    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());

    // Read config
    world->cfg = Config::loadConfig();

    if (forced_mp) {
        // Connect to Multiplayer
#if BUILD_PLAT == BUILD_PSP
        Network::NetworkDriver::get().initGUI();
#endif
        client = create_scopeptr<MP::Client>(world.get(), world->cfg.ip);
        world->client = client.get();
        world->player->client_ref = client.get();
    } else {
        // Try Load Save -- if fails, do generation

        FILE *fptr = fopen((PLATFORM_FILE_PREFIX + "save.ccc").c_str(), "r");
        if (!fptr || !SaveData::load_world(world.get())) {
            if (world->cfg.compat)
                ClassicGenerator::generate(world.get());
            else
                CrossCraftGenerator::generate(world.get());
        }
        if (fptr != nullptr)
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
