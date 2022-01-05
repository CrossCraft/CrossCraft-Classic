#include "ChunkMesh.hpp"
#include <array>

namespace CrossCraft {
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

const std::array<float, 12> bottomFace{0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

const std::array<float, 12> xFace1{
    0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0,
};

const std::array<float, 12> xFace2{
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
};

constexpr uint32_t LIGHT_TOP = 0xFFFFFFFF;
constexpr uint32_t LIGHT_SIDE = 0xFFAAAAAA;
constexpr uint32_t LIGHT_BOT = 0xFF777777;

ChunkMesh::ChunkMesh(int x, int y, int z) : idx_counter(0), tidx_counter(0) {
  cX = x;
  cY = y;
  cZ = z;
}

ChunkMesh::~ChunkMesh() {
  mesh.delete_data();
  transMesh.delete_data();
  idx_counter = 0;
  tidx_counter = 0;
}

struct SurroundPos {
  glm::vec3 up;
  glm::vec3 down;
  glm::vec3 left;
  glm::vec3 right;
  glm::vec3 front;
  glm::vec3 back;

  void update(int x, int y, int z) {
    up = {x, y + 1, z};
    down = {x, y - 1, z};
    left = {x - 1, y, z};
    right = {x + 1, y, z};
    front = {x, y, z + 1};
    back = {x, y, z - 1};
  }
};

void ChunkMesh::generate(const World *wrld) {
  // Max number of faces
  const int numFace = 8192;
  idx_counter = 0;
  m_verts.clear();
  m_index.clear();

  tidx_counter = 0;
  t_verts.clear();
  t_index.clear();

  m_verts.reserve(4 * numFace);
  m_index.reserve(6 * numFace);

  t_verts.reserve(4 * numFace);
  t_index.reserve(6 * numFace);

  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      for (int y = 0; y < 16; y++) {
        int idx =
            ((x + cX * 16) * 256 * 64) + ((z + cZ * 16) * 64) + (y + cY * 16);

        block_t blk = wrld->worldData[idx];

        if (blk == 0) {
          continue;
        }

        SurroundPos surround;
        surround.update(x, y, z);

        tryAddFace(wrld, bottomFace, blk, {x, y, z}, surround.down, LIGHT_BOT);
        tryAddFace(wrld, topFace, blk, {x, y, z}, surround.up, LIGHT_TOP);

        tryAddFace(wrld, leftFace, blk, {x, y, z}, surround.left, LIGHT_SIDE);
        tryAddFace(wrld, rightFace, blk, {x, y, z}, surround.right, LIGHT_SIDE);

        tryAddFace(wrld, frontFace, blk, {x, y, z}, surround.front, LIGHT_SIDE);
        tryAddFace(wrld, backFace, blk, {x, y, z}, surround.back, LIGHT_SIDE);
      }
    }
  }

  m_verts.shrink_to_fit();
  m_index.shrink_to_fit();

  t_verts.shrink_to_fit();
  t_index.shrink_to_fit();

  mesh.add_data(m_verts.data(), m_index.data(), m_index.size());
  transMesh.add_data(t_verts.data(), t_index.data(), t_index.size());
  auto l = m_index.size();
  SC_APP_DEBUG("Number of Indices: {}", l);
}

void ChunkMesh::draw() {
  Rendering::RenderContext::get().matrix_translate({cX * 16, cY * 16, cZ * 16});

  mesh.bind();
  mesh.draw();

  Rendering::RenderContext::get().matrix_clear();
}

void ChunkMesh::drawTransparent() {
  Rendering::RenderContext::get().matrix_translate({cX * 16, cY * 16, cZ * 16});

  transMesh.bind();
  transMesh.draw();

  Rendering::RenderContext::get().matrix_clear();
}

#include <memory>

inline auto getTexture(glm::vec2 sideCount, int index) -> std::array<float, 8> {
  int row = index / (int)sideCount.x;
  int column = index % (int)sideCount.y;

  float sizeX = 1.f / ((float)sideCount.x);
  float sizeY = 1.f / ((float)sideCount.y);
  float y = (float)row * sizeY;
  float x = (float)column * sizeX;
  float h = y + sizeY;
  float w = x + sizeX;

  return {x, y, w, y, w, h, x, h};
}

std::array<float, 8> getTexCoord(uint8_t idx, float lv) {
  return getTexture({16, 16}, 2);
}

void ChunkMesh::tryAddFace(const World *wrld, std::array<float, 12> data,
                           uint8_t blk, glm::vec3 pos, glm::vec3 posCheck,
                           uint32_t lightVal) {
  if (!((posCheck.x == 16 && cX == 8) || (posCheck.x == -1 && cX == 0) ||
        (posCheck.y == -1 && cY == 0) || (posCheck.y == 16 && cY == 8) ||
        (posCheck.z == -1 && cZ == 0) || (posCheck.z == 16 && cZ == 8))) {

    int idx = ((posCheck.x + cX * 16) * 256 * 64) +
              ((posCheck.z + cZ * 16) * 64) + (posCheck.y + cY * 16);

    if (wrld->worldData[idx] == 0) {
      addFaceToMesh(data, getTexCoord(blk, lightVal), pos, lightVal, false);
    }
  } else {
  }
}

void ChunkMesh::addFaceToMesh(std::array<float, 12> data,
                              std::array<float, 8> uv, glm::vec3 pos,
                              uint32_t lightVal, bool trans) {

  auto *m = &m_verts;
  auto *mi = &m_index;
  auto *idc = &idx_counter;

  if (trans) {
    m = &t_verts;
    mi = &t_index;
    idc = &tidx_counter;
  }

  Rendering::Color c;
  c.color = 0xFFFFFFFF;

  for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

    m->emplace_back(Rendering::Vertex{
        uv[tx++],
        uv[tx++],
        c,
        data[idx++] + pos.x,
        data[idx++] + pos.y,
        data[idx++] + pos.z,
    });
  }

  mi->emplace_back((*idc));
  mi->emplace_back((*idc) + 1);
  mi->emplace_back((*idc) + 2);
  mi->emplace_back((*idc) + 2);
  mi->emplace_back((*idc) + 3);
  mi->emplace_back((*idc) + 0);
  (*idc) += 4;
}

} // namespace CrossCraft