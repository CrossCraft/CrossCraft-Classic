#pragma once
#include "World.hpp"
#include <Rendering/Rendering.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class World;

/**
 * Chunk mesh object.
 */
class ChunkMesh {
  public:
    ChunkMesh(int x, int y, int z);
    ~ChunkMesh();

    void generate(const World *wrld);

    void draw();
    void draw_transparent();

  private:
    void reset_allocate();
    void finalize_mesh();

    void try_add_face(const World *wrld, std::array<float, 12> data,
                      uint8_t blk, glm::vec3 pos, glm::vec3 posCheck,
                      uint32_t lightVal);

    void add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          glm::vec3 pos, uint32_t lightVal, bool trans);

    int cX, cY, cZ;

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;

    uint16_t tidx_counter;
    std::vector<Rendering::Vertex> t_verts;
    std::vector<uint16_t> t_index;

    Rendering::Mesh mesh;
    Rendering::Mesh transMesh;
};

} // namespace CrossCraft