#include "ChunkStack.hpp"

namespace CrossCraft {
ChunkStack::ChunkStack(int x, int z) : cX(x), cZ(z) {
    // Create new meshes
    for (int i = 0; i < 4; i++) {
        ChunkMesh *mesh = new ChunkMesh(cX, i, cZ);
        stack[i] = mesh;
    }
}

ChunkStack::~ChunkStack() {
    // Destroy all meshes
    for (int i = 0; i < 4; i++) {
        delete stack[i];
    }
}

auto is_valid(glm::ivec3 ivec) -> bool {
    return ivec.x >= 0 && ivec.x < 256 && ivec.y >= 0 && ivec.y < 256 &&
           ivec.z >= 0 && ivec.z < 256;
}

auto water_can_flow(glm::ivec3 ivec, World *wrld) -> bool {
    for (auto i = ivec.x - 2; i <= ivec.x + 2; i++) {
        for (auto j = ivec.y - 2; j <= ivec.y + 2; j++) {
            for (auto k = ivec.z - 2; k <= ivec.z + 2; k++) {
                auto idx = (i * 256 * 64) + (k * 64) + j;
                if (wrld->worldData[idx] == 19)
                    return false;
            }
        }
    }

    return true;
}

auto ChunkStack::update_check(World *wrld, int blkr, glm::ivec3 chk) -> void {
    if (is_valid(chk)) {
        auto idx = (chk.x * 256 * 64) + (chk.z * 64) + chk.y;

        auto blk = wrld->worldData[idx];
        if (blk == 0) {

            if (blkr == 8 && water_can_flow(chk, wrld)) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                wrld->worldData[idx] = 8;
                wrld->update_lighting(chk.x, chk.z);

                if (wrld->chunks.find(id) != wrld->chunks.end())
                    wrld->chunks[id]->generate(wrld);

                wrld->update_surroundings(chk.x, chk.z);

                updated.push_back(chk);
            }
            else if (blkr == 6 || blkr == 37 || blkr == 38 || blkr == 39 || blkr == 40) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                auto idx = (chk.x * 256 * 64) + (chk.z * 64) + chk.y + 1;
                wrld->worldData[idx] = 0;
                wrld->update_lighting(chk.x, chk.z);

                if (wrld->chunks.find(id) != wrld->chunks.end())
                    wrld->chunks[id]->generate(wrld);

                wrld->update_surroundings(chk.x, chk.z);

                updated.push_back(chk);
            }
        }
    }
}

/**
 * @brief Update Chunk
 *
 * @param wrld The world to reference
 */
void ChunkStack::chunk_update(World *wrld) {
    // Update this chunk

    std::vector<glm::ivec3> newV;

    for (auto &v : posUpdate) {
        newV.push_back(v);
    }

    posUpdate.clear();

    for (auto &pos : newV) {
        auto idx = (pos.x * 256 * 64) + (pos.z * 64) + pos.y;
        auto blk = wrld->worldData[idx];

        if (blk == 8) {
            update_check(wrld, blk, { pos.x, pos.y - 1, pos.z });
            update_check(wrld, blk, { pos.x - 1, pos.y, pos.z });
            update_check(wrld, blk, { pos.x + 1, pos.y, pos.z });
            update_check(wrld, blk, { pos.x, pos.y, pos.z + 1 });
            update_check(wrld, blk, { pos.x, pos.y, pos.z - 1 });
        }
        else if (blk == 6 || blk == 37 || blk == 38 || blk == 39 || blk == 40) {
            update_check(wrld, blk, { pos.x, pos.y - 1, pos.z });
        }
    }

    // Check regens for all
    for (int i = 0; i < 4; i++) {
        if (stack[i]->needsRegen) {
            stack[i]->generate(wrld);
            stack[i]->needsRegen = false;
        }
    }
}

void ChunkStack::post_update(World *wrld) {
    for (auto &v : updated) {
        wrld->update_nearby_blocks(v);
    }

    updated.clear();
}

/**
 * @brief Random Tick Update
 *
 * @param wrld The world to reference
 */
void ChunkStack::rtick_update(World *wrld) {
    // RTick each section
    for (int i = 0; i < 4; i++) {
        stack[i]->rtick(wrld);
    }
}

void ChunkStack::generate(World *wrld) {
    // Generate meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->generate(wrld);
    }
}

void ChunkStack::draw() {
    // Draw meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw();
    }
}

void ChunkStack::draw_transparent() {
    // Draw transparent meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw_transparent();
    }
}

} // namespace CrossCraft
