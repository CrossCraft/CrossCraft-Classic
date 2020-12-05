#include "World.h"
#include "FastNoiseLite.h"
#include <iostream>
#include <GFX/RenderCore.h>
#include <Utilities/Timer.h>
#include <Utilities/Logger.h>

using namespace Stardust;

World::World(std::shared_ptr<Player> p){
	player = p;
	lastPlayerPos = { -1, -1 };
	FastNoiseLite noise;
	noise.SetSeed(rand());
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	float* heightMap = new float[128 * 128];
	memset(worldData, 0, 128 * 128 * 128);
	updatesTilNext = 0;

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			heightMap[y * 128 + x] = (noise.GetNoise(static_cast<float>(x) * 4.0f, static_cast<float>(y) * 4.0f) + 1.0f) / 2.0f * 24.0f + 52.0f;
		}
	}

	for (int z = 0; z < 128; z++) {
		for (int x = 0; x < 128; x++) {
			int height = static_cast<int>(heightMap[z * 128 + x]);

			for (int y = 0; y < 128; y++) {
				int idx = ((y * 128) + z) * 128 + x;

				if (y < height) {
					if (y == 0) {
						worldData[idx] = 37; //bedrock
					}
					else if (y > 0 && y < height - 3) {
						worldData[idx] = 2; //stone
					}
					else if (y < height) {
						worldData[idx] = 3; //dirt
					}
				} else if (y == height) {
					if (y < 63) {
						worldData[idx] = 3; //dirt
					}
					else {
						worldData[idx] = 1; //grass
					}
				}
				else if (y <= 63) {
					worldData[idx] = 7;
				}
				else {
					continue;
				}
			}
		}
	}
	double wt = Utilities::g_AppTimer.deltaTime();
	delete[] heightMap;
	Utilities::app_Logger->info("Generated world in: " + std::to_string(wt));

	terrain_atlas = GFX::g_TextureManager->loadTex("./assets/terrain.png", GFX_FILTER_NEAREST, GFX_FILTER_NEAREST, false);

	double mt = Utilities::g_AppTimer.deltaTime();
	Utilities::app_Logger->info("Generated 1 chunk in: " + std::to_string(mt));
}

World::~World()
{
}

void World::update() {
	glm::ivec2 v = { static_cast<int>(player->pos.x - 8) / 16, static_cast<int>(player->pos.z - 8) / 16};

	if (v != lastPlayerPos) {
		glm::vec2 topLeft = { v.x - 2, v.y - 2 };
		glm::vec2 botRight = { v.x + 2, v.y + 2 };

		std::vector<Vector3i> needed;
		needed.clear();
		std::vector<Vector3i> excess;
		excess.clear();

		for (int x = topLeft.x; x <= botRight.x; x++) {
			for (int z = topLeft.y; z <= botRight.y; z++) {
				needed.push_back({ x, z, 0 });
			}
		}

		for (auto& [pos, chunk] : mesh) {
			bool need = false;
			for (auto& v : needed) {
				if (v == pos) {
					//Is needed
					need = true;
				}
			}

			if (!need) {
				excess.push_back(pos);
			}
		}

		//DIE OLD ONES!
		for (const auto& chk : excess) {
			delete mesh[chk];
			mesh.erase(chk);
		}

		//Make new
		for (const auto& chk : needed) {
			if (mesh.find(chk) == mesh.end()) {
				//NOT FOUND
				if (chk.x >= 0 && chk.x < 8 && chk.y >= 0 && chk.y < 8) {
					remainingGeneration.push_back(chk);
				}
			}
		}

		updatesTilNext = 12;
		lastPlayerPos = v;
	}

	updatesTilNext--;

	if (updatesTilNext < 0 && remainingGeneration.size() > 0) {
		updatesTilNext = 12;

		if (mesh.find(remainingGeneration[0]) == mesh.end()) {
			auto chk = remainingGeneration[0];
			//NOT FOUND
			if (chk.x >= 0 && chk.x < 8 && chk.y >= 0 && chk.y < 8) {
				ChunkStack* chunk = new ChunkStack(chk.x, chk.y);
				chunk->generate(this);
				mesh.emplace(chk, std::move(chunk));
			}
		}

		remainingGeneration.erase(remainingGeneration.begin());
	}

}

void World::draw() {

	GFX::g_TextureManager->bindTex(terrain_atlas);
	//Draw world

	for (auto& [pos, chunk] : mesh) {
		chunk->draw();
	}

	for (auto& [pos, chunk] : mesh) {
		chunk->drawTransparent();
	}
}
