#include "ChunkStack.hpp"

#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

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
            } else if (blkr == Block::Lava) {
                uint16_t x = chk.x / 16;
                uint16_t y = chk.z / 16;
                uint32_t id = x << 16 | (y & 0x00FF);

                wrld->worldData[idx] = 10;
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
        } else if (blk == Block::Water || blk == Block::Still_Water ||
                   blk == Block::Lava || blk == Block::Still_Lava) {
            if (blkr == Block::Gravel || blkr == Block::Sand) {
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

        if ((blk == Block::Water || blk == Block::Still_Water) &&
            blkr == Block::Lava) {
            uint16_t x = chk.x / 16;
            uint16_t y = chk.z / 16;
            uint32_t id = x << 16 | (y & 0x00FF);

            wrld->worldData[idx] = Block::Stone;
            wrld->update_lighting(chk.x, chk.z);

            wrld->update_nearby_blocks({chk.x, chk.y + 1, chk.z});

            if (wrld->chunks.find(id) != wrld->chunks.end())
                wrld->chunks[id]->generate(wrld);

            wrld->update_surroundings(chk.x, chk.z);

            updated.push_back(chk);
        }

        if ((blk == Block::Lava || blk == Block::Still_Lava) &&
            blkr == Block::Water) {
            uint16_t x = chk.x / 16;
            uint16_t y = chk.z / 16;
            uint32_t id = x << 16 | (y & 0x00FF);

            wrld->worldData[idx] = Block::Obsidian;
            wrld->update_lighting(chk.x, chk.z);

            wrld->update_nearby_blocks({chk.x, chk.y + 1, chk.z});

            if (wrld->chunks.find(id) != wrld->chunks.end())
                wrld->chunks[id]->generate(wrld);

            wrld->update_surroundings(chk.x, chk.z);

            updated.push_back(chk);
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

        if (blk == Block::Water || blk == Block::Lava) {
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

            int metaIdx = i * worldSize.z / 16 * worldSize.x / 16 +
                          cZ * worldSize.x / 16 + cX;

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

bool ChunkStack::check_visible(World *wrld, glm::vec3 posCheck, int cY) {
    auto pos = wrld->player->get_pos();
    pos.y -= (1.80f - 1.5965f);

    auto default_vec = pos - glm::vec3(posCheck.x, posCheck.y, posCheck.z);

    const u32 NUM_STEPS = 15;

    default_vec /= static_cast<float>(NUM_STEPS);
    for (u32 c = 0; c < NUM_STEPS; c++) {
        auto cast_pos = pos + (default_vec * static_cast<float>(c));

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        u32 idx = wrld->getIdx(ivec.x, ivec.y, ivec.z);
        if (idx < 0)
            return false;

        auto blk = wrld->worldData[idx];

        if (blk == Block::Air || blk == Block::Water || blk == Block::Leaves ||
            blk == Block::Glass)
            continue;

        if (ivec.x < 0 || ivec.x > wrld->world_size.x || ivec.y < 0 ||
            ivec.y > wrld->world_size.y || ivec.z < 0 ||
            ivec.z > wrld->world_size.z)
            return false;

        auto chkPos = glm::ivec3(ivec.x / 16, ivec.y / 16, ivec.z / 16);

        if (chkPos.x == cX && chkPos.y == cY && chkPos.z == cZ)
            break;

        return false;
    }

    return true;
}

void ChunkStack::draw(World *wrld) {
    // Draw meshes
    for (int i = 0; i < 4; i++) {

        glm::vec2 relative_chunk_pos = glm::vec2(cX * 16.0f, cZ * 16.0f);
        auto diff = glm::vec2(wrld->player->pos.x, wrld->player->pos.z) -
                    relative_chunk_pos;
        auto len = fabsf(sqrtf(diff.x * diff.x + diff.y * diff.y));

        glm::vec4 centerpos =
            glm::vec4(cX * 16 + 8.0f, i * 16, cZ * 16 + 8.0f, 1.0f);

        glm::vec4 res =
            wrld->player->projmat * wrld->player->viewmat * centerpos;

        if (res.w >= 0 || len <= 24.0f) {

#if BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA
            bool visible = false;

            visible =
                visible ||
                check_visible(
                    wrld,
                    glm::vec3(centerpos.x, centerpos.y + 8.0f, centerpos.z), i);

            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 0) * 16, i * 16, (cZ + 0) * 16), i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 1) * 16, i * 16, (cZ + 0) * 16), i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 1) * 16, i * 16, (cZ + 1) * 16), i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 0) * 16, i * 16, (cZ + 1) * 16), i);

            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 0) * 16, (i + 1) * 16, (cZ + 0) * 16),
                    i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 1) * 16, (i + 1) * 16, (cZ + 0) * 16),
                    i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 1) * 16, (i + 1) * 16, (cZ + 1) * 16),
                    i);
            visible =
                visible ||
                check_visible(
                    wrld, glm::vec3((cX + 0) * 16, (i + 1) * 16, (cZ + 1) * 16),
                    i);
#else
            bool visible = true;
#endif

            if (visible || len <= 33.0f)
                stack[i]->draw(ChunkMeshSelection::Opaque);
        }
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
