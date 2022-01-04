#include "World.hpp"
#include <Rendering/Rendering.hpp>
#include <iostream>

#if PSP
#include <pspkernel.h>
#endif

namespace CrossCraft {
World::World(std::shared_ptr<Player> p) {
  player = p;
  lastPlayerPos = {-1, -1};

  terrain_atlas = Rendering::TextureManager::get().load_texture(
      "./assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
      false);

  fsl.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  fsl.SetFrequency(0.001f);
  fsl.SetSeed(time(NULL));

  // Zero the array
  worldData =
      reinterpret_cast<block_t *>(calloc(256 * 64 * 256, sizeof(block_t)));
}

World::~World() {}

void World::update(double dt) {
  // player->update(static_cast<float>(dt));

  // TODO: Update world meshes
}

inline auto range_map(float &input, float curr_range_min, float curr_range_max,
                      float range_min, float range_max) -> void {
  input = (input - curr_range_min) * (range_max - range_min) /
              (curr_range_max - curr_range_min) +
          range_min;
}

auto World::get_noise(float x, float y, NoiseSettings *settings) -> float {

  float amp = settings->amplitude;
  float freq = settings->frequency;

  float sum_noise = 0.0f;
  float sum_amp = 0.0f;

  for (auto i = 0; i < settings->octaves; i++) {
    auto noise = fsl.GetNoise(x * freq, y * freq);

    noise *= amp;
    sum_noise += noise;
    sum_amp += amp;

    amp *= settings->persistence;
    freq *= settings->mod_freq;
  }

  auto divided = sum_noise / sum_amp;
  range_map(divided, -1.0f, 1.0f, settings->range_min, settings->range_max);

  return divided;
}

void World::generate() {

  float *hmap = reinterpret_cast<float *>(malloc(sizeof(float) * 256 * 256));

  NoiseSettings settings = {2, 1.0f, 2.0f, 0.42f, 4.5f, 0.0f, 0.15f, 0.85f};

#if PSP
  auto start_time = sceKernelGetSystemTimeLow();
#endif

  for (int x = 0; x < 256; x++) {
    for (int z = 0; z < 256; z++) {
      hmap[x * 256 + z] = get_noise(x, z, &settings);
    }
  }

#if PSP
  sceKernelDcacheWritebackInvalidateAll();
#endif

  for (int x = 0; x < 256; x++) {
    for (int z = 0; z < 256; z++) {
      int h = hmap[x * 256 + z] * 64.f;
      for (int y = 0; y < h; y++) {
        worldData[(x * 256 * 64) + (z * 64) + y] = 1;
      }
    }
  }

#if PSP
  auto end = sceKernelGetSystemTimeLow();
  auto res = end - start_time;
  printf("[Test]: Took %u microseconds!\n", res);
#endif

  free(hmap);
}

void World::draw() {
  player->draw();
  // Rendering::TextureManager::get().bind_texture(terrain_atlas);

  //  Draw world

  // for (auto &[pos, chunk] : mesh) {
  //   if (player->m_frustum.isBoxInFrustum(chunk->box)) {
  //     chunk->draw();
  //   }
  // }

  // for (auto &[pos, chunk] : mesh) {
  //  if (player->m_frustum.isBoxInFrustum(chunk->box)) {
  //    chunk->drawTransparent();
  //  }
  //}
}

block_t World::getBlock(int x, int y, int z) {
  int idx = ((y * 128) + z) * 128 + x;
  return worldData[idx];
}
} // namespace CrossCraft
