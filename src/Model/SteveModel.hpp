#pragma once
#include "Model.hpp"
#include <glm.hpp>

namespace CrossCraft::Model {
using namespace Stardust_Celeste;

struct SteveData {
    glm::vec3 pos;
    glm::vec2 rot;
    float animTime;
};

class Steve {
  public:
    Steve();
    ~Steve();

    void draw(SteveData *sd);

    inline static auto get() -> Steve & {
        static Steve steve;
        return steve;
    }

  private:
    uint32_t tex;

    Model arm;
    Model leg;
    Model torso;
    Model head;
};
} // namespace CrossCraft::Model