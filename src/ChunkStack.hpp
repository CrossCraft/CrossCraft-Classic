#pragma once
#include "ChunkMesh.hpp"
#include "World.hpp"
#include <array>
#include <glm.hpp>
namespace CrossCraft {

class World;
class ChunkMesh;

class ChunkStack {
  public:
    ChunkStack(int x, int z);
    ~ChunkStack();

    void generate(World *wrld);
    void draw();
    void draw_transparent();

    inline auto get_chunk_pos() -> glm::ivec2 { return {cX, cZ}; }

  private:
    std::array<ChunkMesh *, 4> stack;
    int cX, cZ;
};

} // namespace CrossCraft