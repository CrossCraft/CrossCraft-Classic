#include "Particles.hpp"
#include "../Chunk/ChunkUtil.hpp"

namespace CrossCraft {
ParticleSystem::ParticleSystem(uint32_t tex) : texture(tex) { idx_counter = 0; }
ParticleSystem::~ParticleSystem() {
    mesh.vertices.clear();
    mesh.indices.clear();
}

auto rand_pos() -> float { return ((rand() % 16) - 8) * (1.0f / 16.0f); }
float timer = 0.0f;
auto bind_texture(Particle &particle, uint32_t type) -> void {

    particle.uv = getTexCoord(type, 0xFFCCCCCC);

    const float UV_SIZE = (2.0f / 16.0f) / 16.0f;
    glm::vec2 uv_offset = glm::vec2(particle.uv[6], particle.uv[7]);
    float offset = ((rand() % 12) + 2) * ((1.0f / 16.0f) / 16.0f);
    float offset2 = ((rand() % 12) + 2) * ((1.0f / 16.0f) / 16.0f);
    uv_offset += glm::vec2(offset, offset2);

    // x, h, w, h, w, y, x, y
    //  0 1
    //  1 1
    //  1 0
    //  0 0
    particle.uv[0] = uv_offset.x;

    particle.uv[1] = uv_offset.y + UV_SIZE;
    particle.uv[2] = uv_offset.x + UV_SIZE;
    particle.uv[3] = uv_offset.y + UV_SIZE;
    particle.uv[4] = uv_offset.x + UV_SIZE;

    particle.uv[5] = uv_offset.y;
    particle.uv[6] = uv_offset.x;
    particle.uv[7] = uv_offset.y;
}

void ParticleSystem::initialize(uint32_t type, glm::vec3 pos) {
    particles.clear();
    srand(pos.x + pos.y + pos.z);
    for (int i = 0; i < 32; i++) {

        Particle particle;
        particle.position = pos;
        particle.position.x += rand_pos();
        particle.position.y += rand_pos();
        particle.position.z += rand_pos();
        particle.velocity = glm::vec3(rand() % 10, rand() % 37, rand() % 10);
        particle.velocity /= 10.0f;

        bind_texture(particle, type);

        particles.push_back(particle);
    }

    timer = 0.0f;
}

void ParticleSystem::generate() {
    mesh.delete_data();
    idx_counter = 0;
    mesh.vertices.clear();
    mesh.indices.clear();

    if (particles.size() <= 0)
        return;

    auto cFace = frontFace;

    Rendering::Color c;
    c.color = 0xFFFFFFFF;

    mesh.vertices.push_back(Rendering::Vertex{
        particles[0].uv[0],
        particles[0].uv[1],
        c,
        cFace[0],
        cFace[1],
        cFace[2],
    });

    mesh.vertices.push_back(Rendering::Vertex{
        particles[0].uv[2],
        particles[0].uv[3],
        c,
        cFace[3],
        cFace[4],
        cFace[5],
    });

    mesh.vertices.push_back(Rendering::Vertex{
        particles[0].uv[4],
        particles[0].uv[5],
        c,
        cFace[6],
        cFace[7],
        cFace[8],
    });

    mesh.vertices.push_back(Rendering::Vertex{
        particles[0].uv[6],
        particles[0].uv[7],
        c,
        cFace[9],
        cFace[10],
        cFace[11],
    });

    mesh.indices.push_back(idx_counter);
    mesh.indices.push_back(idx_counter + 1);
    mesh.indices.push_back(idx_counter + 2);
    mesh.indices.push_back(idx_counter + 2);
    mesh.indices.push_back(idx_counter + 3);
    mesh.indices.push_back(idx_counter + 0);
    idx_counter += 4;

    mesh.setup_buffer();
}

void ParticleSystem::update(double dt) {

    timer += dt;

    if (timer < 1.0f) {
        for (auto &p : particles) {
            p.velocity.y -= 16.0f * (float)dt;
            p.position += p.velocity * (float)dt;
        }

        generate();
    }
}
void ParticleSystem::draw(glm::vec3 rot) {
    if (timer < 1.0f) {
        Rendering::TextureManager::get().bind_texture(texture);

#if BUILD_PC || BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_PS2
        glDisable(GL_CULL_FACE);
#else
        sceGuDisable(GU_CULL_FACE);
#endif
        for (auto &p : particles) {

            Rendering::RenderContext::get().matrix_translate(p.position);
            Rendering::RenderContext::get().matrix_scale(
                {0.125f, 0.125f, 0.125f});
            Rendering::RenderContext::get().matrix_scale({0.75f, 0.75f, 0.75f});
            Rendering::RenderContext::get().matrix_rotate({0.0f, -rot.y, 0.0f});
            Rendering::RenderContext::get().matrix_rotate({-rot.x, 0.0f, 0.0f});

            mesh.bind();
            mesh.draw();

            Rendering::RenderContext::get().matrix_clear();
        }
#if BUILD_PC || BUILD_PLAT == BUILD_VITA || BUILD_PLAT == BUILD_PS2
        glEnable(GL_CULL_FACE);
#else
        sceGuEnable(GU_CULL_FACE);
#endif
    }
}

} // namespace CrossCraft
