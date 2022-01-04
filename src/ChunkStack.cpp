#include "ChunkStack.hpp"

namespace CrossCraft {
ChunkStack::ChunkStack(int x, int z) {
  cX = x;
  cZ = z;

  for (int i = 0; i < 4; i++) {
    ChunkMesh *mesh = new ChunkMesh(cX, i, cZ);
    stack[i] = mesh;
  }
}

ChunkStack::~ChunkStack() {
  for (int i = 0; i < 4; i++) {
    delete stack[i];
  }
}

void ChunkStack::generate(World *wrld) {
  for (int i = 0; i < 4; i++) {
    stack[i]->generate(wrld);
  }
}

void ChunkStack::draw() {
  for (int i = 0; i < 4; i++) {
    stack[i]->draw();
  }
}

void ChunkStack::drawTransparent() {
  for (int i = 0; i < 4; i++) {
    stack[i]->drawTransparent();
  }
}

} // namespace CrossCraft