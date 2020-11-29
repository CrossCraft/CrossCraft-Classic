#include "ChunkStack.h"

ChunkStack::ChunkStack(int x, int y)
{
	cX = x;
	cY = y;
	stack.clear();

	for (int i = 0; i < 8; i++) {
		ChunkMesh* mesh = new ChunkMesh(cX, i, cY);
		stack.push_back(mesh);
	}
}

ChunkStack::~ChunkStack()
{
}

void ChunkStack::generate(World* wrld){
	for (int i = 0; i < 8; i++) {
		stack[i]->generate(wrld);
	}
}

void ChunkStack::draw(){
	for (int i = 0; i < 8; i++) {
		stack[i]->draw();
	}
}
