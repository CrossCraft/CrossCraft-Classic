#include "ChunkStack.h"

ChunkStack::ChunkStack(int x, int y)
{
	cX = x;
	cY = y;

	for (int i = 0; i < 8; i++) {
		ChunkMesh* mesh = new ChunkMesh(cX, i, cY);
		stack[i] = mesh;
	}
}

ChunkStack::~ChunkStack()
{
	for (int i = 0; i < 8; i++) {
		delete stack[i];
	}
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

void ChunkStack::drawTransparent()
{
	for (int i = 0; i < 8; i++) {
		stack[i]->drawTransparent();
	}
}
