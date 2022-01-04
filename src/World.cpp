#include "World.hpp"
#include <FastNoiseLite.h>
#include <Rendering/Rendering.hpp>
#include <iostream>

namespace CrossCraft {
World::World(std::shared_ptr<Player> p) {
  player = p;
  lastPlayerPos = {-1, -1};

  terrain_atlas = Rendering::TextureManager::get().load_texture(
      "./assets/terrain.png", SC_TEX_FILTER_NEAREST, SC_TEX_FILTER_NEAREST,
      false);
}

World::~World() {}

void World::update(double dt) {
  player->update(static_cast<float>(dt));
  // TODO: Update world meshes
}

void World::generate() {}

void World::draw() {
  player->draw();
  // GFX::g_TextureManager->bindTex(terrain_atlas);
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
