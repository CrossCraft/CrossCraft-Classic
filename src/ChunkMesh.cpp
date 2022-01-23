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
constexpr uint32_t LIGHT_SIDE = 0xFFCCCCCC;
constexpr uint32_t LIGHT_BOT = 0xFF999999;

constexpr uint32_t LIGHT_TOP_DARK = 0xFFDDDDDD;
constexpr uint32_t LIGHT_SIDE_DARK = 0xFFAAAAAA;
constexpr uint32_t LIGHT_BOT_DARK = 0xFF777777;

ChunkMesh::ChunkMesh(int x, int y, int z)
    : idx_counter(0), tidx_counter(0), cX(x), cY(y), cZ(z) {}

ChunkMesh::~ChunkMesh() {
    // Delete data
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
        // Set all vectors
        up = {x, y + 1, z};
        down = {x, y - 1, z};
        left = {x - 1, y, z};
        right = {x + 1, y, z};
        front = {x, y, z + 1};
        back = {x, y, z - 1};
    }
};

void ChunkMesh::reset_allocate() {
    // Max number of faces
    const int numFace = 8192;

    // Clear
    idx_counter = 0;
    m_verts.clear();
    m_index.clear();

    tidx_counter = 0;
    t_verts.clear();
    t_index.clear();

    // Allocate memory
    m_verts.reserve(4 * numFace);
    m_index.reserve(6 * numFace);

    t_verts.reserve(4 * numFace);
    t_index.reserve(6 * numFace);
}

void ChunkMesh::finalize_mesh() {

    // Cleanup memory
    m_verts.shrink_to_fit();
    m_index.shrink_to_fit();

    t_verts.shrink_to_fit();
    t_index.shrink_to_fit();

    // Add data
    mesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                  m_index.size());
    transMesh.add_data(t_verts.data(), t_verts.size(), t_index.data(),
                       t_index.size());

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
#endif
}

void ChunkMesh::generate(const World *wrld) {

    // Reset + Allocate
    reset_allocate();

    // Loop over the mesh
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 16; y++) {

                int idx = ((x + cX * 16) * 256 * 64) + ((z + cZ * 16) * 64) +
                          (y + cY * 16);

                // Get block
                block_t blk = wrld->worldData[idx];

                // Skip air
                if (blk == 0)
                    continue;

                // Update surrounding positions
                SurroundPos surround;
                surround.update(x, y, z);

                // Add 6 faces

                try_add_face(wrld, bottomFace, blk, {x, y, z}, surround.down,
                             LIGHT_BOT);
                try_add_face(wrld, topFace, blk, {x, y, z}, surround.up,
                             LIGHT_TOP);

                try_add_face(wrld, leftFace, blk, {x, y, z}, surround.left,
                             LIGHT_SIDE);
                try_add_face(wrld, rightFace, blk, {x, y, z}, surround.right,
                             LIGHT_SIDE);

                try_add_face(wrld, frontFace, blk, {x, y, z}, surround.front,
                             LIGHT_SIDE);
                try_add_face(wrld, backFace, blk, {x, y, z}, surround.back,
                             LIGHT_SIDE);
            }
        }
    }

    // Finalize the mesh
    finalize_mesh();
}

void ChunkMesh::draw() {
    // Set matrix
    Rendering::RenderContext::get().matrix_translate(
        {cX * 16, cY * 16, cZ * 16});

    // Draw
    if (mesh.get_index_count() > 0) {
        mesh.bind();
        mesh.draw();
    }

    Rendering::RenderContext::get().matrix_clear();
}

void ChunkMesh::draw_transparent() {
    // Set matrix
    Rendering::RenderContext::get().matrix_translate(
        {cX * 16, cY * 16, cZ * 16});

    // Draw
    if (transMesh.get_index_count() > 0) {
        transMesh.bind();
        transMesh.draw();
    }

    Rendering::RenderContext::get().matrix_clear();
}

#include <memory>

/**
 * @brief Get the Texture coords
 *
 * @param sideCount Number of tiles on XY
 * @param index Index of tile to get
 * @return std::array<float, 8>
 */
inline auto getTexture(glm::vec2 sideCount, int index) -> std::array<float, 8> {
    int row = index / (int)sideCount.x;
    int column = index % (int)sideCount.y;

    float sizeX = 1.f / ((float)sideCount.x);
    float sizeY = 1.f / ((float)sideCount.y);
    float y = (float)row * sizeY;
    float x = (float)column * sizeX;
    float h = y + sizeY;
    float w = x + sizeX;

    return {x, h, w, h, w, y, x, y};
}

/**
 * @brief Get the Tex Coord from a block ID and lightvalue
 *
 * @param idx Index
 * @param lv Light Value
 * @return std::array<float, 8>
 */
std::array<float, 8> getTexCoord(uint8_t idx, uint32_t lv) {
    auto vec = glm::vec2(16, 16);

    if (idx == 1)
        return getTexture(vec, 1);
    else if (idx == 3)
        return getTexture(vec, 2);
    else if (idx == 2) {
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 3);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 2);
        else
            return getTexture(vec, 0);
    } else if (idx == 8)
        return getTexture(vec, 14);
    else if (idx == 17) {
        if (lv == LIGHT_TOP || lv == LIGHT_BOT)
            return getTexture(vec, 21);
        else
            return getTexture(vec, 20);
    } else if (idx == 18)
        return getTexture(vec, 22);

    return getTexture(vec, idx);
}

void ChunkMesh::try_add_face(const World *wrld, std::array<float, 12> data,
                             uint8_t blk, glm::vec3 pos, glm::vec3 posCheck,
                             uint32_t lightVal) {

    // Bounds check
    if (!((posCheck.x == 16 && cX == 15) || (posCheck.x == -1 && cX == 0) ||
          (posCheck.y == -1 && cY == 0) || (posCheck.y == 16 && cY == 15) ||
          (posCheck.z == -1 && cZ == 0) || (posCheck.z == 16 && cZ == 15))) {

        // Calculate block index to peek
        int idx = ((posCheck.x + cX * 16) * 256 * 64) +
                  ((posCheck.z + cZ * 16) * 64) + (posCheck.y + cY * 16);

        int idxl = ((pos.x + cX * 16) * 256 * 4) + ((pos.z + cZ * 16) * 4) + cY;

        auto lv = lightVal;

        if (!((wrld->lightData[idxl] >> (int)pos.y) & 1)) {
            if (lv == LIGHT_TOP)
                lv = LIGHT_TOP_DARK;
            else if (lv == LIGHT_SIDE)
                lv = LIGHT_SIDE_DARK;
            else
                lv = LIGHT_BOT_DARK;
        }

        // Add face to mesh
        if (wrld->worldData[idx] == 0 || wrld->worldData[idx] == 8 ||
            wrld->worldData[idx] == 18) {
            if (blk == 8 && wrld->worldData[idx] != 8) {
                add_face_to_mesh(data, getTexCoord(blk, lightVal), pos, lv,
                                 true);
            } else if (blk == 18) {
                add_face_to_mesh(data, getTexCoord(blk, lightVal), pos, lv,
                                 true);
            } else {
                if (blk != 8)
                    add_face_to_mesh(data, getTexCoord(blk, lightVal), pos, lv,
                                     false);
            }
        }
    }
}

void ChunkMesh::add_face_to_mesh(std::array<float, 12> data,
                                 std::array<float, 8> uv, glm::vec3 pos,
                                 uint32_t lightVal, bool trans) {

    // Set data objects
    auto *m = &m_verts;
    auto *mi = &m_index;
    auto *idc = &idx_counter;

    // If transparent - set data
    if (trans) {
        m = &t_verts;
        mi = &t_index;
        idc = &tidx_counter;
    }

    // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            data[idx++] + pos.x,
            data[idx++] + pos.y,
            data[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;
}

} // namespace CrossCraft