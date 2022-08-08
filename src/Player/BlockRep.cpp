#include "BlockRep.hpp"

namespace CrossCraft {

BlockRep::BlockRep() {
    for (int i = 0; i < 50; i++) {
        setup_model(i);
    }
}
BlockRep::~BlockRep() {}

auto BlockRep::add_face_to_mesh(std::array<float, 12> data,
                                std::array<float, 8> uv, uint32_t lightVal,
                                glm::vec3 mypos, uint8_t type)
    -> void { // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    u8 idx = 0;
    for (int i = 0, tx = 0; i < 4; i++) {

        Rendering::Vertex vert;
        vert.color = c;
        vert.u = uv[tx++];
        vert.v = uv[tx++];
        vert.x = data[idx++] + mypos.x;
        vert.y = data[idx++] + mypos.y;
        vert.z = data[idx++] + mypos.z;

        m_verts[type].push_back(vert);
    }

    // Push Back Indices
    m_index[type].push_back(idx_counter[type]);
    m_index[type].push_back(idx_counter[type] + 1);
    m_index[type].push_back(idx_counter[type] + 2);
    m_index[type].push_back(idx_counter[type] + 2);
    m_index[type].push_back(idx_counter[type] + 3);
    m_index[type].push_back(idx_counter[type] + 0);
    idx_counter[type] += 4;
}

auto BlockRep::setup_model(uint8_t type) -> void {
    idx_counter[type] = 0;
    m_verts[type].clear();
    m_verts[type].shrink_to_fit();
    m_index[type].clear();
    m_index[type].shrink_to_fit();
    blockMesh[type].delete_data();

    glm::vec3 p = {0, 0, 0};

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40) {
        add_face_to_mesh(xFace1, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                         p, type);
        add_face_to_mesh(xFace2, getTexCoord(type, LIGHT_SIDE_X), LIGHT_SIDE_X,
                         p, type);
    } else if (type == Block::Slab) {
        add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP, {0, -0.5f, 0},
            type);
        add_face_to_mesh(leftFaceHalf, getTexCoord(type, LIGHT_SIDE_X), LIGHT_BOT,
            p, type);
        add_face_to_mesh(frontFaceHalf, getTexCoord(type, LIGHT_SIDE_Z),
            LIGHT_SIDE_Z, p, type);
    } else {

        add_face_to_mesh(topFace, getTexCoord(type, LIGHT_TOP), LIGHT_TOP, p,
                         type);
        add_face_to_mesh(leftFace, getTexCoord(type, LIGHT_SIDE_X), LIGHT_BOT,
                         p, type);
        add_face_to_mesh(frontFace, getTexCoord(type, LIGHT_SIDE_Z),
                         LIGHT_SIDE_Z, p, type);
    }

    blockMesh[type].add_data(m_verts[type].data(), m_verts[type].size(),
                             m_index[type].data(), m_index[type].size());
}

auto BlockRep::drawBlk(uint8_t type, int x, int y, int y_offset, float scale) -> void {
    Rendering::RenderContext::get().matrix_view(glm::mat4(1));
  
    Rendering::RenderContext::get().matrix_translate(
        {153.5f + x * 20, (8 + y * 24) + y_offset, -20});

    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40)
        Rendering::RenderContext::get().matrix_rotate({ 0.0f, 45.0f, 0 });
    else
        Rendering::RenderContext::get().matrix_rotate({ 30.0f, 45.0f, 0 });
    Rendering::RenderContext::get().matrix_scale({ scale, scale, scale });
   
    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);
    blockMesh[type].draw();

    Rendering::RenderContext::get().matrix_clear();
}

auto BlockRep::drawBlkHand(uint8_t type, World *wrld, double cube_bob) -> void {
    auto ctx = &Rendering::RenderContext::get();

    ctx->matrix_view(glm::mat4(1.0f));
    ctx->matrix_translate(glm::vec3(0.280f, -0.7225f + cube_bob, -0.725f));
    if (type == 6 || type == 37 || type == 38 || type == 39 || type == 40 ||
        type == 44) {
        ctx->matrix_translate({0.0f, 0.175f, 0.0f});
    }
    ctx->matrix_rotate({0, 45.0f, 0});
    ctx->matrix_scale({0.40f, 0.40f, 0.40f});

    // DISABLE CULL
#if BUILD_PC || BUILD_PLAT == BUILD_VITA
    glClear(GL_DEPTH_BUFFER_BIT);
#else
    sceGuClear(GU_DEPTH_BUFFER_BIT);
#endif

    // Set up texture
    Rendering::TextureManager::get().bind_texture(terrain_atlas);

    bool on_shaded_block;

    int idxl = wrld->getIdxl((int)wrld->player->pos.x, (int)wrld->player->pos.y,
                             (int)wrld->player->pos.z);
    if ((wrld->lightData[idxl] >> ((int)wrld->player->pos.y % 16)) & 1)
        on_shaded_block = false;
    else
        on_shaded_block = true;

    // manipulate the model on the fly to tint when under a shadow
    if (on_shaded_block) {
        for (auto &vert : m_verts[type]) {
            vert.color.rgba.r *= 0.6;
            vert.color.rgba.g *= 0.6;
            vert.color.rgba.b *= 0.6;
        }
        blockMesh[type].delete_data();
        blockMesh[type].add_data(m_verts[type].data(), m_verts[type].size(),
                                 m_index[type].data(), idx_counter[type]);
    }

    blockMesh[type].draw();

    // revert back
    if (on_shaded_block) {
        for (auto &vert : m_verts[type]) {
            vert.color.rgba.r /= 0.6;
            vert.color.rgba.g /= 0.6;
            vert.color.rgba.b /= 0.6;
        }
        blockMesh[type].delete_data();
        blockMesh[type].add_data(m_verts[type].data(), m_verts[type].size(),
                                 m_index[type].data(), idx_counter[type]);
    }


    ctx->matrix_clear();
}

} // namespace CrossCraft
