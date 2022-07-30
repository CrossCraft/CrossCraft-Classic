#include "ChunkStack.hpp"

namespace CrossCraft {
ChunkStack::ChunkStack(int x, int z) : cX(x), cZ(z) {
    // Create new meshes
    for (int i = 0; i < 4; i++) {
        ChunkMesh *mesh = new ChunkMesh(cX, i, cZ);
        stack[i] = mesh;
    }
    border = false;
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
                auto idx = wrld->getIdx(i, j, k);
                if (idx >= 0 && wrld->worldData[idx] == Block::Sponge)
                    return false;
            }
        }
    }

    return true;
}

auto ChunkStack::update_check(World *wrld, int blkr, glm::ivec3 chk) -> void {
    if (is_valid(chk)) {
        auto idx = wrld->getIdx(chk.x, chk.y, chk.z);

        auto blk = wrld->worldData[idx];
        if (blk == Block::Air) {

            if (blkr == Block::Water && water_can_flow(chk, wrld)) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                wrld->worldData[idx] = 8;
                wrld->update_lighting(chk.x, chk.z);

                if (wrld->chunks.find(id) != wrld->chunks.end())
                    wrld->chunks[id]->generate(wrld);

                wrld->update_surroundings(chk.x, chk.z);

                updated.push_back(chk);
            } else if (blkr == Block::Sapling || blkr == Block::Flower1 ||
                       blkr == Block::Flower2 || blkr == Block::Mushroom1 ||
                       blkr == Block::Mushroom2) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                auto idx = wrld->getIdx(chk.x, chk.y + 1, chk.z);
                wrld->worldData[idx] = 0;
                wrld->update_lighting(chk.x, chk.z);

                if (wrld->chunks.find(id) != wrld->chunks.end())
                    wrld->chunks[id]->generate(wrld);

                wrld->update_surroundings(chk.x, chk.z);

                updated.push_back(chk);
            } else if (blkr == Block::Gravel || blkr == Block::Sand) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                auto idx = wrld->getIdx(chk.x, chk.y + 1, chk.z);
                wrld->worldData[idx] = 0;
                idx = wrld->getIdx(chk.x, chk.y, chk.z);

                wrld->worldData[idx] = blkr;
                wrld->update_lighting(chk.x, chk.z);

                wrld->update_nearby_blocks({chk.x, chk.y + 1, chk.z});

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
        auto idx = wrld->getIdx(pos.x, pos.y, pos.z);
        auto blk = wrld->worldData[idx];

        if (blk == Block::Water) {
            update_check(wrld, blk, {pos.x, pos.y - 1, pos.z});
            update_check(wrld, blk, {pos.x - 1, pos.y, pos.z});
            update_check(wrld, blk, {pos.x + 1, pos.y, pos.z});
            update_check(wrld, blk, {pos.x, pos.y, pos.z + 1});
            update_check(wrld, blk, {pos.x, pos.y, pos.z - 1});
        } else if (blk == Block::Sapling || blk == Block::Flower1 ||
                   blk == Block::Flower2 || blk == Block::Mushroom1 ||
                   blk == Block::Mushroom2) {
            update_check(wrld, blk, {pos.x, pos.y - 1, pos.z});
        } else if (blk == Block::Sand || blk == Block::Gravel) {
            update_check(wrld, blk, {pos.x, pos.y - 1, pos.z});
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
        if (stack[i] != nullptr && wrld != nullptr) {
            auto worldSize = wrld->world_size;

            int metaIdx = cX * worldSize.z / 16 * worldSize.y / 16 +
                          cZ * worldSize.y / 16 + i;

            auto meta = wrld->chunksMeta[metaIdx];

            if (!meta.is_empty)
                stack[i]->generate(wrld);
        }
    }
}

void ChunkStack::generate_border() {
    // Generate meshes
    stack[0]->generate_border();
    stack[1]->generate_border();
    stack[2]->generate_blank();
    stack[3]->generate_blank();

    border = true;
}

void ChunkStack::draw() {
    // Draw meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw(ChunkMeshSelection::Opaque);
    }
}

void ChunkStack::draw_transparent() {
    // Draw transparent meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw(ChunkMeshSelection::Transparent);
    }
}

void ChunkStack::draw_flora() {
    // Draw transparent meshes
    for (int i = 0; i < 4; i++) {
        stack[i]->draw(ChunkMeshSelection::Flora);
    }
}

} // namespace CrossCraft
