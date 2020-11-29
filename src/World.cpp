#include "World.h"
#include "FastNoiseLite.h"
#include <iostream>
#include <GFX/RenderCore.h>
#include <Utilities/Timer.h>
#include <Utilities/Logger.h>

using namespace Stardust;

World::World(){
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	float* heightMap = new float[128 * 128];
	memset(worldData, 0, 128 * 128 * 128);

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			heightMap[y * 128 + x] = (noise.GetNoise(static_cast<float>(x), static_cast<float>(y)) + 1.0f) / 2.0f * 32.0f + 48.0f;
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
				else {
					continue;
				}
			}
		}
	}
	float wt = Utilities::g_AppTimer.deltaTime();
	delete[] heightMap;
	Utilities::app_Logger->info("Generated world in: " + std::to_string(wt));

	terrain_atlas = GFX::g_TextureManager->loadTex("./assets/terrain.png", GFX_FILTER_NEAREST, GFX_FILTER_NEAREST, false);

	for (int x = 0; x < 8; x++) {
		for (int z = 0; z < 8; z++) {
			auto m = new ChunkStack(x, z);
			m->generate(this);

			mesh.push_back(m);
		}
	}

	

	float mt = Utilities::g_AppTimer.deltaTime();
	Utilities::app_Logger->info("Generated 1 chunk in: " + std::to_string(mt));
}

World::~World()
{
}

void World::update()
{
}

void World::draw() {

	GFX::g_TextureManager->bindTex(terrain_atlas);
	//Draw world

	for (auto c : mesh) {
		c->draw();
	}
}
