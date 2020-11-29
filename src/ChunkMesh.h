#pragma once
#include <GFX/GFXWrapper.h>
#include "World.h"

using namespace Stardust;

class World;

class ChunkMesh {
public:
	ChunkMesh(int x, int y, int z);
	void generate(const World* wrld);
	void draw();

private:
	void tryAddFace(const World* wrld, std::array<float, 12> data, uint8_t blk, glm::vec3 pos, glm::vec3 posCheck, float lightVal);
	void addFaceToMesh(std::array<float, 12> data, std::array<float, 8> uv, glm::vec3 pos, float lightVal);
	GFX::Mesh mesh;
	GFX::Model model;

	int cX, cY, cZ;
	unsigned int idx_counter;
};