#include "Gamestate.hpp"
#include "Modding/Mod.hpp"
#include "MusicManager.hpp"
#include "Rendering/ShaderManager.hpp"
#include "TexturePackManager.hpp"
#include "Utils.hpp"
#include "World/Generation/ClassicGenerator.hpp"
#include "World/Generation/CrossCraftGenerator.hpp"
#include "World/SaveData.hpp"

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
#elif BUILD_PLAT == BUILD_VITA
const std::string vert_source =
    R"(
    void main ( float3 position, float4 color, float2 uv,
                float2 out vTexcoord : TEXCOORD0, 
                float4 out vPosition : POSITION, 
                float4 out vColor : COLOR, 
                uniform float4x4 proj, 
                uniform float4x4 view, 
                uniform float4x4 model)
    {
        vPosition = mul(mul(mul(float4(position, 1.f), model), view), proj);
        vTexcoord = uv;
        vColor = color;
    }
)";

const std::string frag_source =
    R"(
    float4 main(float2 vTexcoord : TEXCOORD0, float4 vColor : COLOR0, float4 coords : WPOS, uniform sampler2D tex) {

        float4 texColor = tex2D(tex, vTexcoord);
        texColor *= vColor;
        texColor = clamp(texColor, 0.0f, 1.0f);

        float dist = coords.z / coords.w;

        float fogMax = (4.0f * 16.0f * 0.8f);
        float fogMin = (4.0f * 16.0f * 0.2f);
        float fogFactor = (fogMax - dist) / (fogMax - fogMin);
        fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    
        float3 fogColor = float3(0.59765f, 0.796875, 1.0f);
        texColor.rgb = lerp(fogColor.rgb, texColor.rgb, fogFactor);

        if(texColor.a < 0.1f)
            discard;

        return texColor;
    }
)";
#endif

GameState *instanced_gamestate = nullptr;

void GameState::apply_controls() {
    if (instanced_gamestate != nullptr) {
        instanced_gamestate->bind_controls();
    }
}

void GameState::on_start() {
    // Set Color
    Rendering::RenderContext::get().set_color(
        Rendering::Color{0x99, 0xCC, 0xFF, 0xFF});

    int num_mods = Modding::ModManager::get().get_num_mods();
    SC_APP_INFO("Loaded {} mods!", num_mods);

    instanced_gamestate = this;

#if BUILD_PLAT == BUILD_WINDOWS || BUILD_PLAT == BUILD_POSIX ||                \
    BUILD_PLAT == BUILD_VITA
    auto shad =
        Rendering::ShaderManager::get().load_shader(vert_source, frag_source);
    Rendering::ShaderManager::get().bind_shader(shad);
#endif

    Rendering::RenderContext::get().vsync = Option::get().vsync;

    // Make a world and generate it
    world = create_scopeptr<World>(create_refptr<Player>());

    // Read config
    world->cfg = Config::loadConfig();

    if (forced_mp) {
        // Connect to Multiplayer
#if BUILD_PLAT == BUILD_PSP
        Network::NetworkDriver::get().initGUI();
#endif
        client = create_scopeptr<MP::Client>(world.get(), world->cfg.ip,
                                             world->cfg.port);
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
    n3ds_controller = new Input::N3DSController();

    // Bind our controllers
    bind_controls();

    Utilities::Input::add_controller(psp_controller);
    Utilities::Input::add_controller(key_controller);
    Utilities::Input::add_controller(mouse_controller);
    Utilities::Input::add_controller(vita_controller);
    Utilities::Input::add_controller(n3ds_controller);

    Utilities::Input::set_differential_mode("Mouse", true);
    Utilities::Input::set_differential_mode("PSP", true);
    Utilities::Input::set_differential_mode("Vita", true);
    Utilities::Input::set_differential_mode("3DS", true);

    // Request 3D Mode
    Rendering::RenderContext::get().set_mode_3D();

    Modding::ModManager::set_ptr(world.get());
    Modding::ModManager::get().onStart();
}

void GameState::on_cleanup() {
    delete psp_controller;
    delete vita_controller;
    delete key_controller;
    delete mouse_controller;
    delete n3ds_controller;
}

void GameState::quit(std::any d) {
    // Exit application
    auto app = std::any_cast<Core::Application *>(d);
    app->exit();
}

void GameState::on_update(Core::Application *app, double dt) {
    MusicManager::get().update(dt);

    if (client.get() != nullptr)
        client->update(dt);

    if (client.get() == nullptr || client->is_ready) {
        Input::update();
        world->update(dt);
    }

    Modding::ModManager::get().onUpdate();
}
void GameState::on_draw(Core::Application *app, double dt) {
    if (client.get() != nullptr)
        client->draw();

    if (client.get() == nullptr || client->is_ready)
        world->draw();
}
} // namespace CrossCraft
