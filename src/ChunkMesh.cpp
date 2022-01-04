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

constexpr float LIGHT_TOP = 1.0f;
constexpr float LIGHT_SIDE = 0.8f;
constexpr float LIGHT_BOT = 0.6f;

ChunkMesh::ChunkMesh(int x, int y, int z) : idx_counter(0), tidx_counter(0) {
  cX = x;
  cY = y;
  cZ = z;
}

ChunkMesh::~ChunkMesh() {
  // model.deleteData();
  // tmodel.deleteData();
  //
  // idx_counter = 0;
  // mesh.color.clear();
  // mesh.color.shrink_to_fit();
  // mesh.indices.clear();
  // mesh.indices.shrink_to_fit();
  // mesh.position.clear();
  // mesh.position.shrink_to_fit();
  // mesh.uv.clear();
  // mesh.uv.shrink_to_fit();
  //
  // tidx_counter = 0;
  //
  // tmesh.color.clear();
  // tmesh.color.shrink_to_fit();
  // tmesh.indices.clear();
  // tmesh.indices.shrink_to_fit();
  // tmesh.position.clear();
  // tmesh.position.shrink_to_fit();
  // tmesh.uv.clear();
  // tmesh.uv.shrink_to_fit();
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
  //  const int numFace = 8192;
  //
  //  idx_counter = 0;
  //  mesh.color.clear();
  //  mesh.indices.clear();
  //  mesh.position.clear();
  //  mesh.uv.clear();
  //
  //  tidx_counter = 0;
  //  tmesh.color.clear();
  //  tmesh.indices.clear();
  //  tmesh.position.clear();
  //  tmesh.uv.clear();
  //
  //  int ourCidx = ((cY * 8) + cZ) * 8 + cX;
  //
  //  if (!wrld->metaData[ourCidx].isEmpty && !wrld->metaData[ourCidx].isFull) {
  //    mesh.color.reserve(16 * numFace);
  //    mesh.indices.reserve(6 * numFace);
  //    mesh.position.reserve(4 * numFace);
  //    mesh.uv.reserve(4 * numFace);
  //
  //    tmesh.color.reserve(16 * numFace);
  //    tmesh.indices.reserve(6 * numFace);
  //    tmesh.position.reserve(4 * numFace);
  //    tmesh.uv.reserve(4 * numFace);
  //
  //    for (int z = 0; z < 16; z++) {
  //      for (int x = 0; x < 16; x++) {
  //        for (int y = 0; y < 16; y++) {
  //          int idx =
  //              (((y + cY * 16) * 128) + (z + cZ * 16)) * 128 + (x + cX * 16);
  //
  //          uint8_t blk = wrld->worldData[idx];
  //
  //          if (blk == 0) {
  //            continue;
  //          }
  //
  //          if (blk >= 10 && blk <= 14) {
  //            // ADD X TO MESH
  //            continue;
  //          }
  //
  //          SurroundPos surround;
  //          surround.update(x, y, z);
  //
  //          tryAddFace(wrld, bottomFace, blk, {x, y, z}, surround.down,
  //                     LIGHT_BOT);
  //          tryAddFace(wrld, topFace, blk, {x, y, z}, surround.up, LIGHT_TOP);
  //
  //          tryAddFace(wrld, leftFace, blk, {x, y, z}, surround.left,
  //          LIGHT_SIDE); tryAddFace(wrld, rightFace, blk, {x, y, z},
  //          surround.right,
  //                     LIGHT_SIDE);
  //
  //          tryAddFace(wrld, frontFace, blk, {x, y, z}, surround.front,
  //                     LIGHT_SIDE);
  //          tryAddFace(wrld, backFace, blk, {x, y, z}, surround.back,
  //          LIGHT_SIDE);
  //        }
  //      }
  //    }
  //  }
  //
  //  mesh.color.shrink_to_fit();
  //  mesh.indices.shrink_to_fit();
  //  mesh.position.shrink_to_fit();
  //  mesh.uv.shrink_to_fit();
  //  tmesh.color.shrink_to_fit();
  //  tmesh.indices.shrink_to_fit();
  //  tmesh.position.shrink_to_fit();
  //  tmesh.uv.shrink_to_fit();
  //
  //  model.addData(mesh);
  //  tmodel.addData(tmesh);
}

void ChunkMesh::draw() {
  //  GFX::translateModelMatrix({cX * 16, cY * 16, cZ * 16});
  //
  //  model.bind();
  //  model.draw();
  //
  //  GFX::clearModelMatrix();
}
#include <memory>
std::array<float, 8> getTexCoord(uint8_t idx, float lv) {
  // auto atlas = std::make_unique<GFX::TextureAtlas>(static_cast<short>(8));
  //
  // if (idx == 1) {
  //   if (lv == 1.0f) {
  //     return atlas->getTexture(0);
  //   } else if (lv == 0.8f) {
  //     return atlas->getTexture(1);
  //   } else {
  //     return atlas->getTexture(2);
  //   }
  // }
  //
  // if (idx == 3) {
  //   return atlas->getTexture(2);
  // }
  //
  // if (idx == 2) {
  //   return atlas->getTexture(4);
  // }
  //
  // if (idx == 7) {
  //   return atlas->getTexture(37);
  // }
  //
  // return atlas->getTexture(idx);
}

void ChunkMesh::drawTransparent() {
  // GFX::translateModelMatrix(
  //     glm::vec3(cX * 16, (cY * 16) - 1.0f / 8.0f, cZ * 16));
  //
  // tmodel.bind();
  // tmodel.draw();
  //
  // GFX::clearModelMatrix();
}

void ChunkMesh::tryAddFace(const World *wrld, std::array<float, 12> data,
                           uint8_t blk, glm::vec3 pos, glm::vec3 posCheck,
                           float lightVal) {
  // if (!((posCheck.x == 16 && cX == 8) || (posCheck.x == -1 && cX == 0) ||
  //       (posCheck.y == -1 && cY == 0) || (posCheck.y == 16 && cY == 8) ||
  //       (posCheck.z == -1 && cZ == 0) || (posCheck.z == 16 && cZ == 8))) {
  //   int idx = static_cast<int>(
  //       (((posCheck.y + cY * 16) * 128) + (posCheck.z + cZ * 16)) * 128 +
  //       (posCheck.x + cX * 16));
  //
  //  int blkCheck = wrld->worldData[idx];
  //
  //  if (blkCheck == 0 || blkCheck == 7) {
  //    if (blk == 7 && blkCheck != 7) {
  //      addFaceToMesh(data, getTexCoord(blk, lightVal), pos, lightVal, true);
  //    }
  //
  //    if (blk != 7) {
  //      addFaceToMesh(data, getTexCoord(blk, lightVal), pos, lightVal, false);
  //    }
  //  }
  //}
}

void ChunkMesh::addFaceToMesh(std::array<float, 12> data,
                              std::array<float, 8> uv, glm::vec3 pos,
                              float lightVal, bool trans) {

  // auto *m = &mesh;
  //
  // if (trans) {
  //  m = &tmesh;
  //}
  //
  // m->uv.insert(m->uv.end(), uv.begin(), uv.end());
  //
  // for (int i = 0, idx = 0; i < 4; i++) {
  //  m->position.push_back(data[idx++] + pos.x);
  //  m->position.push_back(data[idx++] + pos.y);
  //  m->position.push_back(data[idx++] + pos.z);
  //}
  //
  // m->color.insert(m->color.end(), {
  //                                    lightVal,
  //                                    lightVal,
  //                                    lightVal,
  //                                    1.0f,
  //                                    lightVal,
  //                                    lightVal,
  //                                    lightVal,
  //                                    1.0f,
  //                                    lightVal,
  //                                    lightVal,
  //                                    lightVal,
  //                                    1.0f,
  //                                    lightVal,
  //                                    lightVal,
  //                                    lightVal,
  //                                    1.0f,
  //                                });
  //
  // if (trans) {
  //  m->indices.insert(m->indices.end(), {
  //                                          tidx_counter,
  //                                          tidx_counter + 1,
  //                                          tidx_counter + 2,
  //                                          tidx_counter + 2,
  //                                          tidx_counter + 3,
  //                                          tidx_counter,
  //                                      });
  //  tidx_counter += 4;
  //} else {
  //  m->indices.insert(m->indices.end(), {
  //                                          idx_counter,
  //                                          idx_counter + 1,
  //                                          idx_counter + 2,
  //                                          idx_counter + 2,
  //                                          idx_counter + 3,
  //                                          idx_counter,
  //                                      });
  //  idx_counter += 4;
  //}
} //

} // namespace CrossCraft