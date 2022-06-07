#pragma once
#include "BlockConst.hpp"
#include <Rendering/RenderTypes.hpp>
#include <Rendering/Rendering.hpp>
#include <memory>

using namespace Stardust_Celeste;

namespace CrossCraft {
enum ChunkMeshSelection { Opaque = 1, Transparent = 2, Flora = 3 };

struct SurroundPos {
    glm::vec3 up;
    glm::vec3 down;
    glm::vec3 left;
    glm::vec3 right;
    glm::vec3 front;
    glm::vec3 back;

    inline void update(int x, int y, int z) {
        // Set all vectors
        up = {x, y + 1, z};
        down = {x, y - 1, z};
        left = {x - 1, y, z};
        right = {x + 1, y, z};
        front = {x, y, z + 1};
        back = {x, y, z - 1};
    }
};

struct ChunkMeshInst {
    ~ChunkMeshInst() { delete_data(); }

    inline auto delete_data() -> void {
        mesh.delete_data();
        idx_counter = 0;
        m_verts.clear();
        m_verts.shrink_to_fit();
        m_index.clear();
        m_index.shrink_to_fit();
    }

    inline auto preallocate_memory() -> void {
        const int numFace = 8192;
        m_verts.reserve(4 * numFace);
        m_index.reserve(6 * numFace);
    }

    inline auto finalize() -> void {
        // Cleanup memory
        m_verts.shrink_to_fit();
        m_index.shrink_to_fit();
        // Add data
        mesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                      m_index.size());
    }

    inline auto draw() -> void {
        if (mesh.get_index_count() > 0) {
            mesh.bind();
            mesh.draw();
        }
    }

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh mesh;
};

struct ChunkMeshCollection {
    ChunkMeshInst opaque;
    ChunkMeshInst transparent;
    ChunkMeshInst flora;

    inline auto select(ChunkMeshSelection meshSel) -> ChunkMeshInst * {
        if (meshSel == ChunkMeshSelection::Opaque)
            return &opaque;
        else if (meshSel == ChunkMeshSelection::Transparent)
            return &transparent;
        else if (meshSel == ChunkMeshSelection::Flora)
            return &flora;

        return &opaque;
    }
};

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
inline std::array<float, 8> getTexCoord(uint8_t idx, uint32_t lv) {
    auto vec = glm::vec2(16, 16);

    switch (idx) {
    case 1: // Stone
        return getTexture(vec, 1);
    case 2: // Dirt
        return getTexture(vec, 2);
    case 3: { // Grass
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 3);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 2);
        else
            return getTexture(vec, 0);
    }
    case 4: // Cobblestone
        return getTexture(vec, 16);
    case 5: // Wood
        return getTexture(vec, 4);
    case 7: // Bedrock
        return getTexture(vec, 17);
    case 8:
    case 9: // Water
        return getTexture(vec, 14);
    case 10:
    case 11: // Lava
        return getTexture(vec, 30);
    case 12: // Sand
        return getTexture(vec, 18);
    case 13: // Gravel
        return getTexture(vec, 19);
    case 14: // Gold Ore
        return getTexture(vec, 32);
    case 15: // Iron Ore
        return getTexture(vec, 33);
    case 16: // Coal Ore
        return getTexture(vec, 34);
    case 6: // Sapling
        return getTexture(vec, 15);
    case 17: { // Log
        if (lv == LIGHT_TOP || lv == LIGHT_BOT)
            return getTexture(vec, 21);
        else
            return getTexture(vec, 20);
    }
    case 18: // Leaves
        return getTexture(vec, 22);
    case 19: // Sponge
        return getTexture(vec, 48);
    case 20: // Glass
        return getTexture(vec, 49);
    case 37: // Flower1
        return getTexture(vec, 13);
    case 38: // Flower2
        return getTexture(vec, 12);
    case 39: // Mushroom1
        return getTexture(vec, 29);
    case 40: // Mushroom2
        return getTexture(vec, 28);
    case 41: { // Gold
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 40);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 56);
        else
            return getTexture(vec, 24);
    }
    case 42: { // Iron
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 39);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 55);
        else
            return getTexture(vec, 23);
    }
    case 43: // DSLAB
        return getTexture(vec, 5);
    case 44: { // HSLAB
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 25);
        else
            return getTexture(vec, 6);
    }
    case 45: // Brick
        return getTexture(vec, 7);
    case 46: // TNT
    {
        if (lv == LIGHT_SIDE)
            return getTexture(vec, 8);
        else if (lv == LIGHT_BOT)
            return getTexture(vec, 10);
        else
            return getTexture(vec, 9);
    }
    case 47: { // BookShelf
        if (lv == LIGHT_TOP || lv == LIGHT_BOT)
            return getTexture(vec, 4);
        else
            return getTexture(vec, 35);
    }
    case 48: // Mossy Cobble
        return getTexture(vec, 36);

    case 49: // Obsidian
        return getTexture(vec, 37);

    default:
        return getTexture(vec, idx - 21 + 64);
    }
}

} // namespace CrossCraft