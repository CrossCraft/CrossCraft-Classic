#include "ChunkMesh.hpp"
#include "ChunkUtil.hpp"
#include "World.hpp"
#include <Rendering/Rendering.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {
class World;
class ChunkMesh;

class ChunkMeshBuilder {
  public:
    static void try_add_face(ChunkMesh *chunkMesh, const World *wrld,
                             std::array<float, 12> data, uint8_t blk,
                             glm::vec3 pos, glm::vec3 posCheck,
                             uint32_t lightVal);

    static void add_face_to_mesh(ChunkMesh *chunkMesh,
                                 std::array<float, 12> data,
                                 std::array<float, 8> uv, glm::vec3 pos,
                                 uint32_t lightVal, ChunkMeshSelection meshSel);

    static void add_xface_to_mesh(ChunkMesh *chunkMesh, std::array<float, 8> uv,
                                  glm::vec3 pos, uint32_t lightVal,
                                  const World *wrld);

    static void add_slab_to_mesh(ChunkMesh *chunkMesh, const World *wrld,
                                 uint8_t blk, glm::vec3 pos,
                                 SurroundPos surround);

    static void add_block_to_mesh(ChunkMesh *chunkMesh, const World *wrld,
                                  uint8_t blk, glm::vec3 pos,
                                  SurroundPos surround);
};

} // namespace CrossCraft