#include "ChunkMeshBuilder.hpp"

namespace CrossCraft {

void ChunkMeshBuilder::add_slab_to_mesh(ChunkMesh *chunkMesh, const World *wrld,
                                        uint8_t blk, glm::vec3 pos,
                                        SurroundPos surround) {
    try_add_face(chunkMesh, wrld, bottomFace, blk, {pos.x, pos.y, pos.z},
                 surround.down, LIGHT_BOT);

    // FIXME: have to duplicate and pass world to do a light check.. sigh..
    add_face_to_mesh_wrld(chunkMesh, wrld, topFace, getTexCoord(blk, LIGHT_TOP),
                          {pos.x, pos.y - 0.5f, pos.z}, LIGHT_TOP,
                          ChunkMeshSelection::Opaque);

    try_add_face(chunkMesh, wrld, leftFaceHalf, blk, pos, surround.left,
                 LIGHT_SIDE_X);
    try_add_face(chunkMesh, wrld, rightFaceHalf, blk, pos, surround.right,
                 LIGHT_SIDE_X);

    try_add_face(chunkMesh, wrld, frontFaceHalf, blk, pos, surround.front,
                 LIGHT_SIDE_Z);
    try_add_face(chunkMesh, wrld, backFaceHalf, blk, pos, surround.back,
                 LIGHT_SIDE_Z);
}

void ChunkMeshBuilder::add_block_to_mesh(ChunkMesh *chunkMesh,
                                         const World *wrld, uint8_t blk,
                                         glm::vec3 pos, SurroundPos surround) {

    try_add_face(chunkMesh, wrld, bottomFace, blk, pos, surround.down,
                 LIGHT_BOT);
    try_add_face(chunkMesh, wrld, topFace, blk, pos, surround.up, LIGHT_TOP);

    try_add_face(chunkMesh, wrld, leftFace, blk, pos, surround.left,
                 LIGHT_SIDE_X);
    try_add_face(chunkMesh, wrld, rightFace, blk, pos, surround.right,
                 LIGHT_SIDE_X);

    try_add_face(chunkMesh, wrld, frontFace, blk, pos, surround.front,
                 LIGHT_SIDE_Z);
    try_add_face(chunkMesh, wrld, backFace, blk, pos, surround.back,
                 LIGHT_SIDE_Z);
}

void ChunkMeshBuilder::try_add_face(ChunkMesh *chunkMesh, const World *wrld,
                                    std::array<float, 12> data, uint8_t blk,
                                    glm::vec3 pos, glm::vec3 posCheck,
                                    uint32_t lightVal) {

    // Bounds check
    if (!((posCheck.x == -1 && chunkMesh->cX == 0) ||
          (posCheck.x == 16 &&
           chunkMesh->cX == (wrld->world_size.x / 16 - 1)) ||
          (posCheck.y == -1 && chunkMesh->cY == 0) ||
          (posCheck.y == 16 &&
           chunkMesh->cY == (wrld->world_size.y / 16 - 1)) ||
          (posCheck.z == -1 && chunkMesh->cZ == 0) ||
          (posCheck.z == 16 &&
           chunkMesh->cZ == (wrld->world_size.z / 16 - 1)))) {

        int idxl = ((World *)wrld)
                       ->getIdxl(posCheck.x + chunkMesh->cX * 16,
                                 posCheck.y + chunkMesh->cY * 16,
                                 posCheck.z + chunkMesh->cZ * 16);

        auto lv = lightVal;

        if (idxl >= 0 &&
            !((wrld->lightData[idxl] >> ((int)posCheck.y % 16)) & 1)) {
            switch (lv) {
            case LIGHT_TOP:
                lv = LIGHT_TOP_DARK;
                break;
            case LIGHT_SIDE_X:
                lv = LIGHT_SIDE_X_DARK;
                break;
            case LIGHT_SIDE_Z:
                lv = LIGHT_SIDE_Z_DARK;
                break;
            case LIGHT_BOT:
                lv = LIGHT_BOT_DARK;
                break;
            default:
                lv = LIGHT_BOT_DARK;
                break;
            }
        }

        // Calculate block index to peek
        int idx = ((World *)wrld)
                      ->getIdx(posCheck.x + chunkMesh->cX * 16,
                               posCheck.y + chunkMesh->cY * 16,
                               posCheck.z + chunkMesh->cZ * 16);

        // Add face to mesh
        if (idx >= 0 &&
            idx < (wrld->world_size.x * wrld->world_size.y *
                   wrld->world_size.z) &&
            (wrld->worldData[idx] == Block::Air ||
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
                                 pos, lv, ChunkMeshSelection::Transparent);
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

    int idxl = ((World *)wrld)
                   ->getIdxl(pos.x + chunkMesh->cX * 16, chunkMesh->cY * 16,
                             pos.z + chunkMesh->cZ * 16);

    auto lv = lightVal;
    if (!((wrld->lightData[idxl] >> (int)pos.y) & 1)) {
        switch (lv) {
        case LIGHT_TOP:
            lv = LIGHT_TOP_DARK;
            break;
        case LIGHT_SIDE_X:
            lv = LIGHT_SIDE_X_DARK;
            break;
        case LIGHT_SIDE_Z:
            lv = LIGHT_SIDE_Z_DARK;
            break;
        case LIGHT_BOT:
            lv = LIGHT_BOT_DARK;
            break;
        default:
            lv = LIGHT_BOT_DARK;
            break;
        }
    }

    // Set data objects
    auto mesh =
        chunkMesh->meshCollection.select(ChunkMeshSelection::Transparent);

    auto *m = &mesh->mesh.vertices;
    auto *mi = &mesh->mesh.indices;
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

// TODO: REMOVE ME
void ChunkMeshBuilder::add_face_to_mesh_wrld(ChunkMesh *chunkMesh,
                                             const World *wrld,
                                             std::array<float, 12> data,
                                             std::array<float, 8> uv,
                                             glm::vec3 pos, uint32_t lightVal,
                                             ChunkMeshSelection meshSel) {

    auto mesh = chunkMesh->meshCollection.select(meshSel);

    auto *m = &mesh->mesh.vertices;
    auto *mi = &mesh->mesh.indices;
    auto *idc = &mesh->idx_counter;

    int idxl = ((World *)wrld)
                   ->getIdxl(pos.x + chunkMesh->cX * 16, chunkMesh->cY * 16,
                             pos.z + chunkMesh->cZ * 16);

    auto lv = lightVal;
    if (!((wrld->lightData[idxl] >> (int)pos.y) & 1)) {
        switch (lv) {
        case LIGHT_TOP:
            lv = LIGHT_TOP_DARK;
            break;
        case LIGHT_SIDE_X:
            lv = LIGHT_SIDE_X_DARK;
            break;
        case LIGHT_SIDE_Z:
            lv = LIGHT_SIDE_Z_DARK;
            break;
        case LIGHT_BOT:
            lv = LIGHT_BOT_DARK;
            break;
        default:
            lv = LIGHT_BOT_DARK;
            break;
        }
    }

    // Create color
    Rendering::Color c;
    c.color = lv;

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

void ChunkMeshBuilder::add_face_to_mesh(ChunkMesh *chunkMesh,
                                        std::array<float, 12> data,
                                        std::array<float, 8> uv, glm::vec3 pos,
                                        uint32_t lightVal,
                                        ChunkMeshSelection meshSel) {

    auto mesh = chunkMesh->meshCollection.select(meshSel);

    auto *m = &mesh->mesh.vertices;
    auto *mi = &mesh->mesh.indices;
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
