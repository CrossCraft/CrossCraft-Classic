#pragma once
#include "AABB.h"
#include "ChunkMesh.h"
#include "World.h"
#include <array>

class World;
class ChunkMesh;
class ChunkStack {
  public:
    ChunkStack(int x, int y);
    ~ChunkStack();

    void generate(World *wrld);
    void draw();
    void drawTransparent();

    std::array<ChunkMesh *, 4> stack;
    int cX, cY;

    AABB box;
};