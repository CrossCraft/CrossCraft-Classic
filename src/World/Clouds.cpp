#include "Clouds.hpp"
#include "../TexturePackManager.hpp"
#include <array>
namespace CrossCraft {

Clouds::Clouds() {

    texture = TexturePackManager::get().load_texture(
        "./assets/clouds.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
        true);
    idx_counter = 0;
    scroll = 0.0f;
    generate();
}
Clouds::~Clouds() {
    Rendering::TextureManager::get().delete_texture(texture);
    mesh.vertices.clear();
    mesh.indices.clear();
}

void Clouds::generate() {
    mesh.delete_data();
    idx_counter = 0;
    mesh.vertices.clear();
    mesh.indices.clear();

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {

            const std::array<float, 12> cFace{0, 0, 0, 1, 0, 0,
                                              1, 0, 1, 0, 0, 1};

            const std::array<float, 8> uvs{0, 0, 1, 0, 1, 1, 0, 1};
            Rendering::Color c;
            c.color = 0xBBFFFFFF;

            mesh.vertices.push_back(Rendering::Vertex{
                uvs[0] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[1] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[0] * 16 + x * 16,
                0,
                cFace[2] * 16 + z * 16,
            });

            mesh.vertices.push_back(Rendering::Vertex{
                uvs[2] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[3] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[3] * 16 + x * 16,
                0,
                cFace[5] * 16 + z * 16,
            });

            mesh.vertices.push_back(Rendering::Vertex{
                uvs[4] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[5] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[6] * 16 + x * 16,
                0,
                cFace[8] * 16 + z * 16,
            });

            mesh.vertices.push_back(Rendering::Vertex{
                uvs[6] * (1 / 128.f) + ((float)x / 128.f) + scroll,
                uvs[7] * (1 / 128.f) + ((float)z / 128.f),
                c,
                cFace[9] * 16 + x * 16,
                0,
                cFace[11] * 16 + z * 16,
            });

            mesh.indices.push_back(idx_counter);
            mesh.indices.push_back(idx_counter + 1);
            mesh.indices.push_back(idx_counter + 2);
            mesh.indices.push_back(idx_counter + 2);
            mesh.indices.push_back(idx_counter + 3);
            mesh.indices.push_back(idx_counter + 0);
            idx_counter += 4;
        }
    }

    mesh.setup_buffer();
}

void Clouds::update(double dt) {
    scroll += dt;

    if (scroll > 256)
        scroll = 0.0f;
}

void Clouds::draw() {
    Rendering::RenderContext::get().matrix_translate({scroll, 80, 0});

    Rendering::TextureManager::get().bind_texture(texture);
    mesh.bind();
    mesh.draw();

    Rendering::RenderContext::get().matrix_clear();
}
} // namespace CrossCraft