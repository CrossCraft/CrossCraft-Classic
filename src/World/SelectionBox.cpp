#include "SelectionBox.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
namespace CrossCraft {
SelectionBox::SelectionBox() {
    idx_counter = 0;
    m_verts.clear();
    m_verts.shrink_to_fit();
    m_index.clear();
    m_index.shrink_to_fit();
    blockMesh.delete_data();

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    m_index.push_back(0);
    m_index.push_back(1);
    m_index.push_back(2);
    m_index.push_back(3);
    m_index.push_back(0);

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    m_index.push_back(4);
    m_index.push_back(5);
    m_index.push_back(6);
    m_index.push_back(7);
    m_index.push_back(4);

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    m_index.push_back(8);
    m_index.push_back(9);
    m_index.push_back(10);
    m_index.push_back(11);
    m_index.push_back(8);

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});

    m_index.push_back(12);
    m_index.push_back(13);
    m_index.push_back(14);
    m_index.push_back(15);
    m_index.push_back(12);

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});

    m_index.push_back(16);
    m_index.push_back(17);
    m_index.push_back(18);
    m_index.push_back(19);
    m_index.push_back(16);

    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    m_verts.push_back(Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    m_index.push_back(20);
    m_index.push_back(21);
    m_index.push_back(22);
    m_index.push_back(23);
    m_index.push_back(20);

    blockMesh.add_data(m_verts.data(), m_verts.size(), m_index.data(),
                       m_index.size());
};

auto SelectionBox::update_position(World *wrld) -> void {
    position = glm::ivec3(0, 0, 0);
    auto pos = wrld->player->get_pos();
    pos.y -= (1.80f - 1.5965f);

    auto default_vec = glm::vec3(0, 0, 1);
    default_vec =
        glm::rotateX(default_vec, DEGTORAD(wrld->player->get_rot().x));
    default_vec =
        glm::rotateY(default_vec, DEGTORAD(-wrld->player->get_rot().y + 180));

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

        u32 idx = wrld->getIdx(ivec.x, ivec.y, ivec.z);
        if (idx < 0)
            return;

        auto blk = wrld->worldData[idx];

        if (blk == Block::Air || blk == Block::Bedrock || blk == Block::Water ||
            blk == Block::Lava)
            continue;

        if (ivec.x < 0 || ivec.x > wrld->world_size.x || ivec.y < 0 ||
            ivec.y > wrld->world_size.y || ivec.z < 0 ||
            ivec.z > wrld->world_size.z)
            return;

        position = ivec;
        break;
    }
}

auto SelectionBox::draw() -> void {
    auto ctx = &Rendering::RenderContext::get();

    ctx->matrix_translate(glm::vec3(position.x, position.y, position.z));
    blockMesh.draw_lines();

    ctx->matrix_clear();
    return;
}
} // namespace CrossCraft
