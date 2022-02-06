#pragma once
#include <Rendering/Rendering.hpp>
using namespace Stardust_Celeste;

namespace CrossCraft {

class Clouds {
  public:
    Clouds();
    ~Clouds();

    void update(double dt);
    void draw();

  private:
    void generate();

    unsigned int texture;

    uint16_t idx_counter;
    float scroll;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;

    Rendering::Mesh mesh;
};
} // namespace CrossCraft