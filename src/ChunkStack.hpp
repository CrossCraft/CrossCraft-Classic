#pragma once
#include "ChunkMesh.hpp"
#include "World.hpp"
#include <array>

namespace CrossCraft {

class World;
class ChunkMesh;

class ChunkStack {
  public:
    ChunkStack(int x, int z);
    ~ChunkStack();

    void generate(World *wrld);
    void draw();
    void drawTransparent();

    std::array<ChunkMesh *, 4> stack;
    int cX, cZ;
};

} // namespace CrossCraft