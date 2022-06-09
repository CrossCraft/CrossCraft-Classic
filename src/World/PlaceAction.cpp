#include "PlaceAction.hpp"
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto PlaceAction::place(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

    if (w->place_icd < 0)
        w->place_icd = 0.2f;
    else
        return;

    auto pos = w->player->get_pos();

    if (w->player->in_inventory)
        return;

    auto pos_ivec = glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y),
                               static_cast<s32>(pos.z));

    if (!validate_ivec3(pos_ivec))
        return;

    auto pidx = (pos_ivec.x * 256 * 64) + (pos_ivec.z * 64) + pos_ivec.y;
    if (w->worldData[pidx] != 0 && w->worldData[pidx] != 8)
        return;

    auto default_vec = glm::vec3(0, 0, 1);

    default_vec = glm::rotateX(default_vec, DEGTORAD(w->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-w->player->get_rot().y + 180));

    const float REACH_DISTANCE = 4.0f;
    default_vec *= REACH_DISTANCE;

    const u32 NUM_STEPS = 50;

    for (u32 i = 0; i < NUM_STEPS; i++) {
        float percentage =
            static_cast<float>(i) / static_cast<float>(NUM_STEPS);

        auto cast_pos = pos + (default_vec * percentage);

        auto ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                               static_cast<s32>(cast_pos.y),
                               static_cast<s32>(cast_pos.z));

        auto posivec =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y),
                       static_cast<s32>(pos.z));
        auto posivec2 =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y - 1),
                       static_cast<s32>(pos.z));
        auto posivec3 =
            glm::ivec3(static_cast<s32>(pos.x), static_cast<s32>(pos.y - 1.8f),
                       static_cast<s32>(pos.z));

        if (!validate_ivec3(ivec) || ivec == posivec)
            continue;

        u32 idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;
        auto blk = w->worldData[idx];

        if (blk == Block::Air || blk == Block::Water)
            continue;

        cast_pos = pos + (default_vec * static_cast<float>(i - 1) /
                          static_cast<float>(NUM_STEPS));

        ivec = glm::ivec3(static_cast<s32>(cast_pos.x),
                          static_cast<s32>(cast_pos.y),
                          static_cast<s32>(cast_pos.z));

        if (!validate_ivec3(ivec))
            return;

        auto bk = w->player->itemSelections[w->player->selectorIDX];
        if ((ivec == posivec || ivec == posivec2 || ivec == posivec3) &&
            (bk != 6 && bk != 37 && bk != 38 && bk != 39 && bk != 40))
            return;

        idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;

        auto idx2 = (ivec.x * 256 * 64) + (ivec.z * 64) + (ivec.y - 1);

        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

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
        if (bk == Block::Sponge) {
            for (auto i = ivec.x - 2; i <= ivec.x + 2; i++) {
                for (auto j = ivec.y - 2; j <= ivec.y + 2; j++) {
                    for (auto k = ivec.z - 2; k <= ivec.z + 2; k++) {
                        idx = (i * 256 * 64) + (k * 64) + j;

                        // If it's water or flowing water, replace with air.
                        if (w->worldData[idx] == Block::Water ||
                            w->worldData[idx] == Block::Still_Water) {
                            w->worldData[idx] = Block::Air;

                            w->update_surroundings(i, k);
                        }
                    }
                }
            }
        }

        // Update Lighting
        w->update_lighting(ivec.x, ivec.z);

        if (w->chunks.find(id) != w->chunks.end())
            w->chunks[id]->generate(w);

        w->update_surroundings(ivec.x, ivec.z);
        w->update_nearby_blocks(ivec);

        return;
        break;
    }
}

} // namespace CrossCraft