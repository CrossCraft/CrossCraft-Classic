#include "Clouds.hpp"
#include <array>
namespace CrossCraft {

Clouds::Clouds() {

    texture = Rendering::TextureManager::get().load_texture(
        "./assets/clouds.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true);
    idx_counter = 0;
    scroll = 0.0f;
    generate();
}
Clouds::~Clouds() {
    Rendering::TextureManager::get().delete_texture(texture);
    m_verts.clear();
    m_index.clear();
}

void Clouds::generate() {
    mesh.delete_data();
    idx_counter = 0;
    m_verts.clear();
    m_index.clear();

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {

            const std::array<float, 12> cFace{0, 0, 0, 1, 0, 0,
                                              1, 0, 1, 0, 0, 1};

            const std::array<float, 8> uvs{0, 0, 1, 0, 1, 1, 0, 1};
            Rendering::Color c;
            c.color = 0xBBFFFFFF;

            m_verts.push_back(Rendering::Vertex{
                uvs[0] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[1] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[0] * 16 + x * 16,
                0,
                cFace[2] * 16 + z * 16,
            });

            m_verts.push_back(Rendering::Vertex{
                uvs[2] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[3] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[3] * 16 + x * 16,
                0,
                cFace[5] * 16 + z * 16,
            });

            m_verts.push_back(Rendering::Vertex{
                uvs[4] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[5] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[6] * 16 + x * 16,
                0,
                cFace[8] * 16 + z * 16,
            });

            m_verts.push_back(Rendering::Vertex{
                uvs[6] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[7] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[9] * 16 + x * 16,
                0,
                cFace[11] * 16 + z * 16,
            });

            m_index.push_back(idx_counter);
            m_index.push_back(idx_counter + 1);
            m_index.push_back(idx_counter + 2);
            m_index.push_back(idx_counter + 2);
            m_index.push_back(idx_counter + 3);
            m_index.push_back(idx_counter + 0);
            idx_counter += 4;
        }
    }

    mesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                  m_index.size());
}

void Clouds::update(double dt) {
    scroll += dt / 1024.f;
    generate();
}

void Clouds::draw() {
    Rendering::RenderContext::get().matrix_translate({0, 80, 0});

    Rendering::TextureManager::get().bind_texture(texture);
    mesh.bind();
    mesh.draw();

    Rendering::RenderContext::get().matrix_clear();
}
} // namespace CrossCraft