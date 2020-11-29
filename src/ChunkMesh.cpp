#include "ChunkMesh.h"
#include <array>

const std::array<float, 12> frontFace{
	0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1,
};

const std::array<float, 12> backFace{
	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
};

const std::array<float, 12> leftFace{
	0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,
};

const std::array<float, 12> rightFace{
	1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
};

const std::array<float, 12> topFace{
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0,
};

const std::array<float, 12> bottomFace{ 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 };

const std::array<float, 12> xFace1{
	0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0,
};

const std::array<float, 12> xFace2{
	0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
};

constexpr float LIGHT_TOP = 1.0f;
constexpr float LIGHT_SIDE = 0.8f;
constexpr float LIGHT_BOT = 0.6f;

ChunkMesh::ChunkMesh(int x, int y, int z)
{
	cX = x;
	cY = y;
	cZ = z;
}

struct SurroundPos {
	glm::vec3 up;
	glm::vec3 down;
	glm::vec3 left;
	glm::vec3 right;
	glm::vec3 front;
	glm::vec3 back;

	void update(int x, int y, int z) {
		up = { x, y + 1, z };
		down = { x, y - 1, z };
		left = { x - 1, y, z };
		right = { x + 1, y, z };
		front = { x, y, z + 1 };
		back = { x, y, z - 1 };
	}
};

void ChunkMesh::generate(const World* wrld){
	idx_counter = 0;
	mesh.color.clear();
	mesh.indices.clear();
	mesh.position.clear();
	mesh.uv.clear();

	for (int z = 0; z < 16; z++) {
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				int idx = (((y + cY * 16) * 128) + (z + cZ * 16)) * 128 + (x + cX * 16);

				uint8_t blk = wrld->worldData[idx];

				if (blk == 0) {
					continue;
				}

				if (blk >= 10 && blk <= 14) {
					//ADD X TO MESH
					continue;
				}

				SurroundPos surround;
				surround.update(x, y, z);

				tryAddFace(wrld, bottomFace, blk, { x, y, z }, surround.down, LIGHT_BOT);
				tryAddFace(wrld, topFace, blk, { x, y, z }, surround.up, LIGHT_TOP);

				tryAddFace(wrld, leftFace, blk, { x, y, z }, surround.left, LIGHT_SIDE);
				tryAddFace(wrld, rightFace, blk, { x, y, z }, surround.right, LIGHT_SIDE);

				tryAddFace(wrld, frontFace, blk, { x, y, z }, surround.front, LIGHT_SIDE);
				tryAddFace(wrld, backFace, blk, { x, y, z }, surround.back, LIGHT_SIDE);
			}
		}
	}

	model.addData(mesh);
}

void ChunkMesh::draw(){
	GFX::translateModelMatrix({ cX * 16, cY * 16, cZ * 16 });

	model.bind();
	model.draw();

	GFX::clearModelMatrix();

}
#include <memory>
std::array<float, 8> getTexCoord(uint8_t idx, float lv) {
	auto atlas = std::make_unique<GFX::TextureAtlas>(8);

	if (idx == 1) {
		if (lv == 1.0f) {
			return atlas->getTexture(0);
		}
		else if (lv == 0.8f) {
			return atlas->getTexture(1);
		}
		else {
			return atlas->getTexture(2);
		}
	}

	if (idx == 3) {
		return atlas->getTexture(2);
	}

	if (idx == 2) {
		return atlas->getTexture(4);
	}

	return atlas->getTexture(idx);
}

void ChunkMesh::tryAddFace(const World* wrld, std::array<float, 12> data, uint8_t blk, glm::vec3 pos, glm::vec3 posCheck, float lightVal){
	if (!((posCheck.x == 16 && cX == 8) || (posCheck.x == -1 && cX == 0) || (posCheck.y == -1 && cY == 0) || (posCheck.y == 16 && cY == 8) || (posCheck.z == -1 && cZ == 0) || (posCheck.z == 16 && cZ == 8))) {
		int idx = (((posCheck.y + cY * 16) * 128) + (posCheck.z + cZ * 16)) * 128 + (posCheck.x + cX * 16);
		
		int blkCheck = wrld->worldData[idx];
		
		if (blkCheck == 0) {
			addFaceToMesh(data, getTexCoord(blk, lightVal), pos, lightVal);
		}
	}
}

void ChunkMesh::addFaceToMesh(std::array<float, 12> data, std::array<float, 8> uv, glm::vec3 pos, float lightVal){
	mesh.uv.insert(mesh.uv.end(), uv.begin(), uv.end());

	for (int i = 0, idx = 0; i < 4; i++) {
		mesh.position.push_back(data[idx++] + pos.x);
		mesh.position.push_back(data[idx++] + pos.y);
		mesh.position.push_back(data[idx++] + pos.z);
	}

	mesh.color.insert(mesh.color.end(), {
		lightVal, lightVal, lightVal, 1.0f,
		lightVal, lightVal, lightVal, 1.0f,
		lightVal, lightVal, lightVal, 1.0f,
		lightVal, lightVal, lightVal, 1.0f,
		});

	mesh.indices.insert(mesh.indices.end(), {
		idx_counter, idx_counter + 1, idx_counter + 2,
		idx_counter + 2, idx_counter + 3, idx_counter,
		});

	idx_counter += 4;
}
