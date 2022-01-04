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
  void drawTransparent();

private:
  void tryAddFace(const World *wrld, std::array<float, 12> data, uint8_t blk,
                  glm::vec3 pos, glm::vec3 posCheck, float lightVal);

  void addFaceToMesh(std::array<float, 12> data, std::array<float, 8> uv,
                     glm::vec3 pos, float lightVal, bool trans);

  Rendering::Mesh mesh;
  Rendering::Mesh transMesh;

  int cX, cY, cZ;
  unsigned int idx_counter;
  unsigned int tidx_counter;
};

} // namespace CrossCraft