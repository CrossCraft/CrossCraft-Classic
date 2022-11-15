#include "SelectionBox.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

template <typename T> constexpr T DEGTORAD(T x) { return x / 180.0f * 3.14159; }
namespace CrossCraft {
SelectionBox::SelectionBox() {
    idx_counter = 0;
    blockMesh.vertices.clear();
    blockMesh.vertices.shrink_to_fit();
    blockMesh.indices.clear();
    blockMesh.indices.shrink_to_fit();
    blockMesh.delete_data();

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    blockMesh.indices.push_back(0);
    blockMesh.indices.push_back(1);
    blockMesh.indices.push_back(2);
    blockMesh.indices.push_back(3);
    blockMesh.indices.push_back(0);

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    blockMesh.indices.push_back(4);
    blockMesh.indices.push_back(5);
    blockMesh.indices.push_back(6);
    blockMesh.indices.push_back(7);
    blockMesh.indices.push_back(4);

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});

    blockMesh.indices.push_back(8);
    blockMesh.indices.push_back(9);
    blockMesh.indices.push_back(10);
    blockMesh.indices.push_back(11);
    blockMesh.indices.push_back(8);

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});

    blockMesh.indices.push_back(12);
    blockMesh.indices.push_back(13);
    blockMesh.indices.push_back(14);
    blockMesh.indices.push_back(15);
    blockMesh.indices.push_back(12);

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 0});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 0});

    blockMesh.indices.push_back(16);
    blockMesh.indices.push_back(17);
    blockMesh.indices.push_back(18);
    blockMesh.indices.push_back(19);
    blockMesh.indices.push_back(16);

    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 0, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 0, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 1, 1, 1});
    blockMesh.vertices.push_back(
        Rendering::Vertex{0, 0, {255, 255, 255, 255}, 0, 1, 1});

    blockMesh.indices.push_back(20);
    blockMesh.indices.push_back(21);
    blockMesh.indices.push_back(22);
    blockMesh.indices.push_back(23);
    blockMesh.indices.push_back(20);

    blockMesh.setup_buffer();
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
    blockMesh.draw(Rendering::PRIM_TYPE_LINE);

    ctx->matrix_clear();
    return;
}
} // namespace CrossCraft
