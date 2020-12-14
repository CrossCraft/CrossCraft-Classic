/*****************************************************************//**
 * \file   ChunkStack.h
 * \brief  A stack of 8 chunk mesh pointers
 * 
 * \author Iridescence - Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * \date   December 2020
 *********************************************************************/
#pragma once
#include "ChunkMesh.h"
#include "World.h"
#include <array>
#include "AABB.h"

class World;
class ChunkMesh;
class ChunkStack {
public:
	ChunkStack(int x, int y);
	~ChunkStack();

	void generate(World* wrld);
	void draw();
	void drawTransparent();

	std::array<ChunkMesh*, 8> stack;
	int cX, cY;

	AABB box;
};