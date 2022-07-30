#include "PlaceAction.hpp"
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto PlaceAction::place(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

    // Check if we can place
    if (w->place_icd < 0)
        w->place_icd = 0.2f;
    else
        return;

    // If in inventory, skip
    if (w->player->in_inventory)
        return;

    // Setup default camera pointing vector
    auto default_vec = glm::vec3(0, 0, 1);
    default_vec = glm::rotateX(default_vec, DEGTORAD(w->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-w->player->get_rot().y + 180));

    // Vector has reach
    const float REACH_DISTANCE = 5.0f;
    default_vec *= REACH_DISTANCE;

    // Iterate over vector to check for placement
    const u32 NUM_STEPS = 50;
    for (u32 i = 0; i < NUM_STEPS; i++) {
        float percentage =
            static_cast<float>(i) / static_cast<float>(NUM_STEPS);

        auto cast_pos = w->player->get_pos() + (default_vec * percentage);

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        auto pos = w->player->get_pos();

        // Validate ivec is in bounds
        if (!validate_ivec3(ivec))
            continue;

        // Get block
        u32 idx = w->getIdx(ivec.x, ivec.y, ivec.z);
        auto blk = w->worldData[idx];

        // Pass through these blocks
        if (blk == Block::Air || blk == Block::Water || blk == Block::Lava)
            continue;

        cast_pos = pos + (default_vec * static_cast<float>(i - 1) /
                          static_cast<float>(NUM_STEPS));

        ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                          static_cast<s32>(cast_pos.y),
                          static_cast<s32>(cast_pos.z));

        auto copy_ivec = ivec;

        // Check your feet
        auto bk = w->player->itemSelections[w->player->selectorIDX];
        float xm = -0.3f;
        for (int i = 0; i < 2; i++) {
            float ym = -0.3f;
            for (int c = 0; c < 2; c++) {

                auto posivec = glm::ivec3(static_cast<s32>(pos.x + xm),
                                          static_cast<s32>(pos.y),
                                          static_cast<s32>(pos.z + ym));
                auto posivec2 = glm::ivec3(static_cast<s32>(pos.x + xm),
                                           static_cast<s32>(pos.y - 1),
                                           static_cast<s32>(pos.z + ym));
                auto posivec3 = glm::ivec3(static_cast<s32>(pos.x + xm),
                                           static_cast<s32>(pos.y - 1.8f),
                                           static_cast<s32>(pos.z + ym));

                ivec = glm::ivec3(static_cast<s32>(cast_pos.x + xm),
                                  static_cast<s32>(cast_pos.y),
                                  static_cast<s32>(cast_pos.z + ym));

                if (!validate_ivec3(ivec))
                    return;

                if ((ivec == posivec || ivec == posivec2 || ivec == posivec3) &&
                    (bk != Block::Sapling && bk != Block::Flower1 &&
                     bk != Block::Flower2 && bk != Block::Mushroom1 &&
                     bk != Block::Mushroom2))
                    return;

                ym += 0.3f;
            }
            xm += 0.3f;
        }

        auto posivec =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y),
                       static_cast<s32>(pos.z));

        ivec = copy_ivec;
        // Enforce block placement rules
        idx = w->getIdx(ivec.x, ivec.y, ivec.z);

        auto idx2 = w->getIdx(ivec.x, ivec.y - 1, ivec.z);
        blk = w->player->itemSelections[w->player->selectorIDX];

        auto blk2 = w->worldData[idx2];

        if ((blk == Block::Flower1 || blk == Block::Flower2) &&
            blk2 != Block::Grass)
            return;

        if (blk == Block::Sapling &&
            (blk2 != Block::Grass && blk2 != Block::Dirt))
            return;

        if ((blk == Block::Mushroom1 || blk == Block::Mushroom2) &&
            (blk2 != Block::Stone && blk2 != Block::Cobblestone &&
             blk2 != Block::Gravel))
            return;

        w->worldData[idx] = blk;

        // Drain water in a surrounding 5x5x5 area if a sponge was placed.

        if (w->client == nullptr) {
            if (bk == Block::Sponge) {
                for (auto i = ivec.x - 2; i <= ivec.x + 2; i++) {
                    for (auto j = ivec.y - 2; j <= ivec.y + 2; j++) {
                        for (auto k = ivec.z - 2; k <= ivec.z + 2; k++) {
                            idx = (i * 256 * 64) + (k * 64) + j;

                            // If it's water or flowing water, replace with air.
                            if (idx >= 0 && idx < (256 * 64 * 256) &&
                                    w->worldData[idx] == Block::Water ||
                                w->worldData[idx] == Block::Still_Water) {
                                w->worldData[idx] = Block::Air;

                                w->update_surroundings(i, k);
                            }
                        }
                    }
                }
            }
        }

        // Update metadata
        int mIdx = ivec.x / 16 * w->world_size.z / 16 * w->world_size.y / 16 +
                   ivec.z / 16 * w->world_size.y / 16 + ivec.y / 16;

        w->chunksMeta[mIdx].is_empty = false;

        // Update client if necessary
        if (w->client != nullptr) {
            w->set_block(ivec.x, ivec.y, ivec.z, 1,
                         w->player->itemSelections[w->player->selectorIDX]);
        }

        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

        // Update Lighting
        w->update_lighting(ivec.x, ivec.z);

        if (w->chunks.find(id) != w->chunks.end())
            w->chunks[id]->generate(w);

        w->update_surroundings(ivec.x, ivec.z);
        w->update_nearby_blocks(ivec);

        return;
    }
}

} // namespace CrossCraft