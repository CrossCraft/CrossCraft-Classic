#include "Particles.hpp"
#include "../Chunk/ChunkUtil.hpp"

namespace CrossCraft {
ParticleSystem::ParticleSystem(uint32_t tex) : texture(tex) { idx_counter = 0; }
ParticleSystem::~ParticleSystem() {
    m_verts.clear();
    m_index.clear();
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
    m_verts.clear();
    m_index.clear();

    for (auto &p : particles) {
        const std::array<float, 12> cFace{
            0,            // 0
            0,            // 1
            0,            // 2
            0,            // 3
            1.0f / 16.0f, // 4
            0,            // 5
            1.0f / 16.0f, // 6
            1.0f / 16.0f, // 7
            1.0f / 16.0f, // 8
            1.0f / 16.0f, // 9
            0,            // 10
            1.0f / 16.0f, // 11
        };

        Rendering::Color c;
        c.color = 0xFFFFFFFF;

        m_verts.push_back(Rendering::Vertex{
            p.uv[0],
            p.uv[1],
            c,
            cFace[0] + p.position.x,
            cFace[1] + p.position.y,
            cFace[2] + p.position.z,
        });

        m_verts.push_back(Rendering::Vertex{
            p.uv[2],
            p.uv[3],
            c,
            cFace[3] + p.position.x,
            cFace[4] + p.position.y,
            cFace[5] + p.position.z,
        });

        m_verts.push_back(Rendering::Vertex{
            p.uv[4],
            p.uv[5],
            c,
            cFace[6] + p.position.x,
            cFace[7] + p.position.y,
            cFace[8] + p.position.z,
        });

        m_verts.push_back(Rendering::Vertex{
            p.uv[6],
            p.uv[7],
            c,
            cFace[9] + p.position.x,
            cFace[10] + p.position.y,
            cFace[11] + p.position.z,
        });

        m_index.push_back(idx_counter);
        m_index.push_back(idx_counter + 1);
        m_index.push_back(idx_counter + 2);
        m_index.push_back(idx_counter + 2);
        m_index.push_back(idx_counter + 3);
        m_index.push_back(idx_counter + 0);
        idx_counter += 4;
    }

    mesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                  m_index.size());
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
void ParticleSystem::draw() {
    if (timer < 1.0f) {
        Rendering::TextureManager::get().bind_texture(texture);
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glDisable(GL_CULL_FACE);
#else
        sceGuDisable(GU_CULL_FACE);
#endif

        mesh.bind();
        mesh.draw();

#if BUILD_PC || BUILD_PLAT == BUILD_VITA
        glEnable(GL_CULL_FACE);
#else
        sceGuEnable(GU_CULL_FACE);
#endif
    }
}

} // namespace CrossCraft
