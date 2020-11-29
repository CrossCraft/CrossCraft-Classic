#pragma once
#include "ChunkMesh.h"
#include "World.h"
class World;
class ChunkMesh;
class ChunkStack {
public:
	ChunkStack(int x, int y);
	~ChunkStack();

	void generate(World* wrld);
	void draw();

	std::vector<ChunkMesh*> stack;
	int cX, cY;
};