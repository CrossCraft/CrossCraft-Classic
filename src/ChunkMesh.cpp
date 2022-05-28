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

const std::array<float, 12> frontFaceHalf{
    0, 0, 1, 1, 0, 1, 1, 0.5f, 1, 0, 0.5f, 1,
};

const std::array<float, 12> backFaceHalf{
    1, 0, 0, 0, 0, 0, 0, 0.5f, 0, 1, 0.5f, 0,
};

const std::array<float, 12> leftFaceHalf{
    0, 0, 0, 0, 0, 1, 0, 0.5f, 1, 0, 0.5f, 0,
};

const std::array<float, 12> rightFaceHalf{
    1, 0, 1, 1, 0, 0, 1, 0.5f, 0, 1, 0.5f, 1,
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
    : idx_counter(0), tidx_counter(0), cX(x), cY(y), cZ(z), rtcounter(0),
      needsRegen(0) {}

ChunkMesh::~ChunkMesh() {
    // Delete data
    mesh.delete_data();
    transMesh.delete_data();

    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();

    tidx_counter = 0;
    t_verts.clear();
    t_verts.shrink_to_fit();
    t_index.clear();
    t_index.shrink_to_fit();
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
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();

    tidx_counter = 0;
    t_verts.clear();
    t_verts.shrink_to_fit();
    t_index.clear();
    t_index.shrink_to_fit();

    // Allocate memory
    m_verts.reserve(4 * numFace);
    m_index.reserve(6 * numFace);

    t_verts.reserve(4 * numFace);
    t_index.reserve(6 * numFace);

    mesh.delete_data();
    transMesh.delete_data();
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

/*
0   Air             X
1   Stone           X
2   Dirt            X
3   Grass           X
4   Cobblestone     X
5   Wood            X
6   Sapling         X
7   Bedrock         X
8   Water           X
9   Still Water     X
10  Lava            X
11  Still Lava      X
12  Sand            X
13  Gravel          X
14  Gold Ore        X
15  Iron Ore        X
16  Coal Ore        X
17  Logs            X
18  Leaves          X
19  Sponge          X
20  Glass           X
21 - 36 Wool variants
37  Flower1         X
38  Flower2         X
39  Mushroom1       X
40  Mushroom2       X
41  Gold            X
42  Iron            X
43  Double Slab     X
44  Slab            X
45  Brick           X
46  TNT             X
47  Bookshelf       X
48  Mossy Rocks     X
49  Obsidian        X
*/

/**
 * @brief Get the Tex Coord from a block ID and lightvalue
 *
 * @param idx Index
 * @param lv Light Value
 * @return std::array<float, 8>
 */
std::array<float, 8> ChunkMesh::getTexCoord(uint8_t idx, uint32_t lv) {
    auto vec = glm::vec2(16, 16);

    if (idx == 1) // Stone
        return getTexture(vec, 1);
    else if (idx == 2) // Dirt
        return getTexture(vec, 2);
    else if (idx == 3) { // Grass
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 3);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 2);
        else
            return getTexture(vec, 0);
    } else if (idx == 4) // Cobblestone
        return getTexture(vec, 16);
    else if (idx == 5) // Wood
        return getTexture(vec, 4);
    else if (idx == 7) // Bedrock
        return getTexture(vec, 17);
    else if (idx == 8 || idx == 9) // Water
        return getTexture(vec, 14);
    else if (idx == 10 || idx == 11) // Lava
        return getTexture(vec, 30);
    else if (idx == 12) // Sand
        return getTexture(vec, 18);
    else if (idx == 13) // Gravel
        return getTexture(vec, 19);
    else if (idx == 14) // Gold Ore
        return getTexture(vec, 32);
    else if (idx == 15) // Iron Ore
        return getTexture(vec, 33);
    else if (idx == 16) // Coal Ore
        return getTexture(vec, 34);
    else if (idx == 6) // Sapling
        return getTexture(vec, 15);
    else if (idx == 17) { // Log
        if (lv == LIGHT_TOP || lv == LIGHT_BOT)
            return getTexture(vec, 21);
        else
            return getTexture(vec, 20);
    } else if (idx == 18) // Leaves
        return getTexture(vec, 22);
    else if (idx == 19) // Sponge
        return getTexture(vec, 48);
    else if (idx == 20) // Glass
        return getTexture(vec, 49);
    else if (idx == 37) // Flower1
        return getTexture(vec, 13);
    else if (idx == 38) // Flower2
        return getTexture(vec, 12);
    else if (idx == 39) // Mushroom1
        return getTexture(vec, 29);
    else if (idx == 40) // Mushroom2
        return getTexture(vec, 28);
    else if (idx == 41) // Gold
        return getTexture(vec, 24);
    else if (idx == 42) // Iron
        return getTexture(vec, 23);
    else if (idx == 43) // DSLAB
        return getTexture(vec, 5);
    else if (idx == 44) // HSLAB
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 25);
        else
            return getTexture(vec, 6);
    else if (idx == 45) // Brick
        return getTexture(vec, 7);
    else if (idx == 46) // TNT
    {
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 8);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 10);
        else
            return getTexture(vec, 9);
    } else if (idx == 47) // BookShelf
        if (lv == LIGHT_TOP || lv == LIGHT_BOT)
            return getTexture(vec, 4);
        else
            return getTexture(vec, 35);
    else if (idx == 48) // Mossy Cobble
        return getTexture(vec, 36);
    else if (idx == 49) // Obsidian
        return getTexture(vec, 37);
    else {
        return getTexture(vec, idx - 21 + 64);
    }
}

void ChunkMesh::rtick(World *wrld) {
    srand(rtcounter++ + cX * cZ << cY);
    int x = rand() % 16 + cX * 16;
    int y = rand() % 16 + cY * 16;
    int z = rand() % 16 + cZ * 16;

    auto idx = (x * 256 * 64) + (z * 64) + y;

    y += 1;
    if (y >= 64)
        return;

    auto idx2 = (x * 256 * 64) + (z * 64) + y;
    auto blk2 = wrld->worldData[idx2];
    auto blk = wrld->worldData[idx];

    if (blk == 3 && blk2 != 0) {
        wrld->worldData[idx] = 2;
        needsRegen = true;
    }

    if (blk == 2 && blk2 == 0) {
        wrld->worldData[idx] = 3;
        needsRegen = true;
    }
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

                if (blk == 37 || blk == 38 || blk == 39 || blk == 40 ||
                    blk == 6) {
                    add_xface_to_mesh(getTexCoord(blk, LIGHT_TOP), {x, y, z},
                                      LIGHT_TOP);
                    continue;
                }

                // Update surrounding positions
                SurroundPos surround;
                surround.update(x, y, z);

                // Add 6 faces

                if (blk == 44) {

                    try_add_face(wrld, bottomFace, blk, {x, y, z},
                                 surround.down, LIGHT_BOT);
                    add_face_to_mesh(topFace, getTexCoord(blk, LIGHT_TOP),
                                     {x, y - 0.5f, z}, LIGHT_TOP, false);

                    try_add_face(wrld, leftFaceHalf, blk, {x, y, z},
                                 surround.left, LIGHT_SIDE);
                    try_add_face(wrld, rightFaceHalf, blk, {x, y, z},
                                 surround.right, LIGHT_SIDE);

                    try_add_face(wrld, frontFaceHalf, blk, {x, y, z},
                                 surround.front, LIGHT_SIDE);
                    try_add_face(wrld, backFaceHalf, blk, {x, y, z},
                                 surround.back, LIGHT_SIDE);

                    continue;
                }

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
            wrld->worldData[idx] == 18 || wrld->worldData[idx] == 37 ||
            wrld->worldData[idx] == 38 || wrld->worldData[idx] == 39 ||
            wrld->worldData[idx] == 40 || wrld->worldData[idx] == 6 ||
            wrld->worldData[idx] == 20 || wrld->worldData[idx] == 44) {
            if (blk == 8 && wrld->worldData[idx] != 8) {
                std::array<float, 12> data2 = data;
                data2[1] *= 0.9f;
                data2[4] *= 0.9f;
                data2[7] *= 0.9f;
                data2[10] *= 0.9f;

                add_face_to_mesh(data2, getTexCoord(blk, lightVal), pos, lv,
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

void ChunkMesh::add_xface_to_mesh(std::array<float, 8> uv, glm::vec3 pos,
                                  uint32_t lightVal) {

    // Set data objects
    auto *m = &t_verts;
    auto *mi = &t_index;
    auto *idc = &tidx_counter;

    // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            xFace1[idx++] + pos.x,
            xFace1[idx++] + pos.y,
            xFace1[idx++] + pos.z,
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

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            xFace2[idx++] + pos.x,
            xFace2[idx++] + pos.y,
            xFace2[idx++] + pos.z,
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