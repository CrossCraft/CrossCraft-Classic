#include "DigAction.hpp"
#include <Utilities/Input.hpp>
#include <gtx/rotate_vector.hpp>

namespace CrossCraft {

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }

auto DigAction::dig(std::any d) -> void {
    auto w = std::any_cast<World *>(d);

    if (w->break_icd < 0)
        w->break_icd = 0.2f;
    else
        return;

    auto pos = w->player->get_pos();
    auto default_vec = glm::vec3(0, 0, 1);

    if (w->player->in_inventory) {
        using namespace Stardust_Celeste::Utilities;
        auto cX = (Input::get_axis("Mouse", "X") + 1.0f) / 2.0f;
        auto cY = (Input::get_axis("Mouse", "Y") + 1.0f) / 2.0f;

#if PSP
        cX = w->player->vcursor_x / 480.0f;
        cY = 1.0f - (w->player->vcursor_y / 272.0f);
#endif

        if (cX > 0.3125f && cX < 0.675f)
            cX = (cX - 0.3125f) / .04f;
        else
            return;

        if (cY > 0.3125f && cY < 0.7188f)
            cY = (cY - 0.3125f) / .08f;
        else
            return;

        int iX = cX;
        int iY = cY;

        int idx = iY * 9 + iX;

        if (idx > 41)
            return;

        w->player->itemSelections[w->player->selectorIDX] =
            w->player->inventorySelection[idx];

        return;
    }

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

        if (!validate_ivec3(ivec))
            continue;

        u32 idx = (ivec.x * 256 * 64) + (ivec.z * 64) + ivec.y;
        auto blk = w->worldData[idx];

        if (blk == 0 || blk == 7 || blk == 8)
            continue;

        w->psystem->initialize(blk, cast_pos);

        uint16_t x = ivec.x / 16;
        uint16_t y = ivec.z / 16;
        uint32_t id = x << 16 | (y & 0x00FF);

        bool was_sponge = false;
        if (w->worldData[idx] == 19) {
            was_sponge = true;
        }

        w->worldData[idx] = 0;

        // Update surrounding blocks on a larger radius for water filling
        if (was_sponge == true) {
            for (auto i = ivec.x - 3; i <= ivec.x + 3; i++) {
                for (auto j = ivec.z - 3; j <= ivec.z + 3; j++) {
                    w->add_update({i, ivec.y, j});
                    w->add_update({i, ivec.y + 1, j});
                    w->add_update({i, ivec.y - 1, j});
                    w->add_update({i, ivec.y + 2, j});
                    w->add_update({i, ivec.y - 2, j});
                }
            }
        }

        // Update Lighting
        w->update_lighting(ivec.x, ivec.z);

        if (w->chunks.find(id) != w->chunks.end())
            w->chunks[id]->generate(w);

        w->update_surroundings(ivec.x, ivec.z);
        w->update_nearby_blocks(ivec);

        break;
    }
}

} // namespace CrossCraft