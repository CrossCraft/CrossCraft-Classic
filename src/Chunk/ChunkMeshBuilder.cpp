#include "ChunkMeshBuilder.hpp"

namespace CrossCraft {

void ChunkMeshBuilder::add_slab_to_mesh(ChunkMesh *chunkMesh, const World *wrld,
                                        uint8_t blk, glm::vec3 pos,
                                        SurroundPos surround) {
    try_add_face(chunkMesh, wrld, bottomFace, blk, {pos.x, pos.y, pos.z},
                 surround.down, LIGHT_BOT);
    add_face_to_mesh(chunkMesh, topFace, getTexCoord(blk, LIGHT_TOP),
                     {pos.x, pos.y - 0.5f, pos.z}, LIGHT_TOP,
                     ChunkMeshSelection::Opaque);

    try_add_face(chunkMesh, wrld, leftFaceHalf, blk, pos, surround.left,
                 LIGHT_SIDE);
    try_add_face(chunkMesh, wrld, rightFaceHalf, blk, pos, surround.right,
                 LIGHT_SIDE);

    try_add_face(chunkMesh, wrld, frontFaceHalf, blk, pos, surround.front,
                 LIGHT_SIDE);
    try_add_face(chunkMesh, wrld, backFaceHalf, blk, pos, surround.back,
                 LIGHT_SIDE);
}

void ChunkMeshBuilder::add_block_to_mesh(ChunkMesh *chunkMesh,
                                         const World *wrld, uint8_t blk,
                                         glm::vec3 pos, SurroundPos surround) {

    try_add_face(chunkMesh, wrld, bottomFace, blk, pos, surround.down,
                 LIGHT_BOT);
    try_add_face(chunkMesh, wrld, topFace, blk, pos, surround.up, LIGHT_TOP);

    try_add_face(chunkMesh, wrld, leftFace, blk, pos, surround.left,
                 LIGHT_SIDE);
    try_add_face(chunkMesh, wrld, rightFace, blk, pos, surround.right,
                 LIGHT_SIDE);

    try_add_face(chunkMesh, wrld, frontFace, blk, pos, surround.front,
                 LIGHT_SIDE);
    try_add_face(chunkMesh, wrld, backFace, blk, pos, surround.back,
                 LIGHT_SIDE);
}

void ChunkMeshBuilder::try_add_face(ChunkMesh *chunkMesh, const World *wrld,
                                    std::array<float, 12> data, uint8_t blk,
                                    glm::vec3 pos, glm::vec3 posCheck,
                                    uint32_t lightVal) {

    // Bounds check
    if (!((posCheck.x == 16 && chunkMesh->cX == 15) ||
          (posCheck.x == -1 && chunkMesh->cX == 0) ||
          (posCheck.y == -1 && chunkMesh->cY == 0) ||
          (posCheck.y == 16 && chunkMesh->cY == 15) ||
          (posCheck.z == -1 && chunkMesh->cZ == 0) ||
          (posCheck.z == 16 && chunkMesh->cZ == 15))) {


        int idxl = ((posCheck.x + chunkMesh->cX * 16) * 256 * 4) +
            ((posCheck.z + chunkMesh->cZ * 16) * 4) +
            (posCheck.y + chunkMesh->cY * 16) / 16;

        auto lv = lightVal;

        if (idxl >= 0 && !((wrld->lightData[idxl] >> ((int)posCheck.y % 16)) & 1)) {
            if (lv == LIGHT_TOP)
                lv = LIGHT_TOP_DARK;
            else if (lv == LIGHT_SIDE)
                lv = LIGHT_SIDE_DARK;
            else
                lv = LIGHT_BOT_DARK;
        }


        // Calculate block index to peek
        int idx = ((posCheck.x + chunkMesh->cX * 16) * 256 * 64) +
                  ((posCheck.z + chunkMesh->cZ * 16) * 64) +
                  (posCheck.y + chunkMesh->cY * 16);
        // Add face to mesh
        if (idx >= 0 && idx < (256 * 64 * 256) && (wrld->worldData[idx] == Block::Air ||
            wrld->worldData[idx] == Block::Water ||
            wrld->worldData[idx] == Block::Still_Water ||
#ifndef PSP
            wrld->worldData[idx] == Block::Leaves ||
#endif
            wrld->worldData[idx] == Block::Flower1 ||
            wrld->worldData[idx] == Block::Flower2 ||
            wrld->worldData[idx] == Block::Mushroom1 ||
            wrld->worldData[idx] == Block::Mushroom2 ||
            wrld->worldData[idx] == Block::Sapling ||
            wrld->worldData[idx] == Block::Glass ||
            wrld->worldData[idx] == Block::Slab)) {
            if (blk == Block::Water && wrld->worldData[idx] != Block::Water) {
                std::array<float, 12> data2 = data;
                if (data == topFace) {
                    data2[1] *= 0.9f;
                    data2[4] *= 0.9f;
                    data2[7] *= 0.9f;
                    data2[10] *= 0.9f;
                }
                add_face_to_mesh(chunkMesh, data2, getTexCoord(blk, lightVal),
                                 pos, lv, ChunkMeshSelection::Transparent);

            } else if (blk == Block::Leaves) {
                add_face_to_mesh(chunkMesh, data, getTexCoord(blk, lightVal),
                                 pos, lv, ChunkMeshSelection::Opaque);
            } else if (blk == Block::Glass &&
                       wrld->worldData[idx] != Block::Glass) {
                add_face_to_mesh(chunkMesh, data, getTexCoord(blk, lightVal),
                                 pos, lv, ChunkMeshSelection::Flora);
            } else {
                if (blk != Block::Water && blk != Block::Glass)
                    add_face_to_mesh(chunkMesh, data,
                                     getTexCoord(blk, lightVal), pos, lv,
                                     ChunkMeshSelection::Opaque);
            }
        }
    }
}

void ChunkMeshBuilder::add_xface_to_mesh(ChunkMesh *chunkMesh,
                                         std::array<float, 8> uv, glm::vec3 pos,
                                         uint32_t lightVal, const World *wrld) {

    int idxl = ((pos.x + chunkMesh->cX * 16) * 256 * 4) +
               ((pos.z + chunkMesh->cZ * 16) * 4) + chunkMesh->cY;

    auto lv = lightVal;
    if (!((wrld->lightData[idxl] >> (int)pos.y) & 1)) {
        if (lv == LIGHT_TOP)
            lv = LIGHT_TOP_DARK;
        else if (lv == LIGHT_SIDE)
            lv = LIGHT_SIDE_DARK;
        else
            lv = LIGHT_BOT_DARK;
    }

    // Set data objects
    auto mesh =
        chunkMesh->meshCollection.select(ChunkMeshSelection::Transparent);

    auto *m = &mesh->m_verts;
    auto *mi = &mesh->m_index;
    auto *idc = &mesh->idx_counter;

    // Create color
    Rendering::Color c;
    c.color = lv;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            xFace1[idx++] + pos.x,
            xFace1[idx++] + pos.y,
            xFace1[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            xFace2[idx++] + pos.x,
            xFace2[idx++] + pos.y,
            xFace2[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;

    // UV ORDER
    //{x, h, w, h, w, y, x, y};
    std::array<float, 8> uv2{uv[2], uv[3], uv[4], uv[5],
                             uv[6], uv[7], uv[0], uv[1]};

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv2[tx++],
            uv2[tx++],
            c,
            xFace3[idx++] + pos.x,
            xFace3[idx++] + pos.y,
            xFace3[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv2[tx++],
            uv2[tx++],
            c,
            xFace4[idx++] + pos.x,
            xFace4[idx++] + pos.y,
            xFace4[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;
}

void ChunkMeshBuilder::add_face_to_mesh(ChunkMesh *chunkMesh,
                                        std::array<float, 12> data,
                                        std::array<float, 8> uv, glm::vec3 pos,
                                        uint32_t lightVal,
                                        ChunkMeshSelection meshSel) {

    auto mesh = chunkMesh->meshCollection.select(meshSel);

    auto *m = &mesh->m_verts;
    auto *mi = &mesh->m_index;
    auto *idc = &mesh->idx_counter;

    // Create color
    Rendering::Color c;
    c.color = lightVal;

    // Push Back Verts
    for (int i = 0, tx = 0, idx = 0; i < 4; i++) {

        m->push_back(Rendering::Vertex{
            uv[tx++],
            uv[tx++],
            c,
            data[idx++] + pos.x,
            data[idx++] + pos.y,
            data[idx++] + pos.z,
        });
    }

    // Push Back Indices
    mi->push_back((*idc));
    mi->push_back((*idc) + 1);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 2);
    mi->push_back((*idc) + 3);
    mi->push_back((*idc) + 0);
    (*idc) += 4;
}

} // namespace CrossCraft