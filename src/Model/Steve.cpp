#include "../TexturePackManager.hpp"
#include "../Utils.hpp"
#include "SteveModel.hpp"
#include <yaml-cpp/yaml.h>

namespace CrossCraft::Model {

Steve::Steve() {
    YAML::Node config =
        YAML::LoadFile(PLATFORM_FILE_PREFIX + "texturepacks/default/assets/" +
                       "model/steve.yaml");

    auto tSize = config["model"][0]["texsize"];
    auto texSize = glm::vec2(tSize[0].as<float>(), tSize[1].as<float>());

    arm.load(config["model"][1], texSize);
    leg.load(config["model"][2], texSize);
    torso.load(config["model"][3], texSize);
    head.load(config["model"][4], texSize);

    tex = TexturePackManager::get().load_texture(
        "assets/char.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST, false,
        false);
}

Steve::~Steve() {}

void Steve::draw(SteveData *sd) {
    auto ctx = &Rendering::RenderContext::get();
    ctx->matrix_clear();
    ctx->matrix_translate({sd->pos.x, sd->pos.y, sd->pos.z});
    ctx->matrix_rotate(glm::vec3(sd->rot, 0.0f));
    ctx->matrix_scale({0.9f, 0.9f, 0.9f});

    Rendering::TextureManager::get().bind_texture(tex);

    auto sval = sinf(sd->animTime * 3.14159f * 4.0f / 3.0f) * 30.0f;

    head.draw({0.0f, 0.0f + 1.8f, 0.0f}, {0, 0, 0}, {1, 1, 1});
    arm.draw({0.0f, -0.25f + 1.8f, 0.375f}, {0, 180, sval}, {-1, 1, 1});
    arm.draw({0.0f, -0.25f + 1.8f, -0.375f}, {0, 0, sval}, {1, 1, 1});
    torso.draw({0.0f, -0.25f + 1.8f, 0.0f}, {0, 0, 0}, {1, 1, 1});
    leg.draw({0.0f, -1.0f + 1.8f, 0.125f}, {0, 180, sval}, {-1, 1, 1});
    leg.draw({0.0f, -1.0f + 1.8f, -0.125f}, {0, 0, sval}, {1, 1, 1});
    ctx->matrix_clear();
}
} // namespace CrossCraft::Model
