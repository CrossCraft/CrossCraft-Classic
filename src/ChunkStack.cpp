#include "ChunkStack.hpp"

namespace CrossCraft {
ChunkStack::ChunkStack(int x, int z) : cX(x), cZ(z) {
    // Create new meshes
    for (int i = 0; i < 4; i++) {
        ChunkMesh *mesh = new ChunkMesh(cX, i, cZ);
        stack[i] = mesh;
    }
}

ChunkStack::~ChunkStack() {
    // Destroy all meshes
    for (int i = 0; i < 4; i++) {
        delete stack[i];
    }
}

void ChunkStack::generate(World *wrld) {
    // Generate meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->generate(wrld);
    }
}

void ChunkStack::draw() {
    // Draw meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw();
    }
}

void ChunkStack::draw_transparent() {
    // Draw transparent meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw_transparent();
    }
}

} // namespace CrossCraft