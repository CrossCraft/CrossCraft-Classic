#include "ChunkMesh.hpp"
#include "ChunkMeshBuilder.hpp"

namespace CrossCraft {

ChunkMesh::ChunkMesh(int x, int y, int z)
    : cX(x), cY(y), cZ(z), rtcounter(0), needsRegen(0) {}

ChunkMesh::~ChunkMesh() {}

void ChunkMesh::reset_allocate() {
    meshCollection.transparent.delete_data();
    meshCollection.flora.delete_data();
    meshCollection.opaque.delete_data();
    meshCollection.opaque.preallocate_memory();
}

void ChunkMesh::finalize_mesh() {

    meshCollection.transparent.finalize();
    meshCollection.flora.finalize();
    meshCollection.opaque.finalize();

#if PSP
    sceKernelDcacheWritebackInvalidateAll();
#endif
}

void ChunkMesh::rtick(World *wrld) {
    int x = rand() % 16; // + cX * 16;
    int y = rand() % 16; // + cY * 16;
    int z = rand() % 16; // + cZ * 16;

    int idxl = ((x + cX * 16) * 256 * 4) + ((z + cZ * 16) * 4) + cY;
    auto is_dark = (!((wrld->lightData[idxl] >> (int)y) & 1));

    x += cX * 16;
    y += cY * 16;
    z += cZ * 16;

    auto idx = (x * 256 * 64) + (z * 64) + y;

    y += 1;
    if (y >= 64)
        return;

    auto idx2 = (x * 256 * 64) + (z * 64) + y;
    auto blk2 = wrld->worldData[idx2];
    auto blk = wrld->worldData[idx];

    auto blk2_is_valid_grass =
        (blk2 == Block::Air || blk2 == Block::Sapling ||
         blk2 == Block::Flower1 || blk2 == Block::Flower2 ||
         blk2 == Block::Mushroom1 || blk2 == Block::Mushroom2);

    if (blk == Block::Grass && !blk2_is_valid_grass) {
        wrld->worldData[idx] = Block::Dirt;
        needsRegen = true;
    }

    if (blk == Block::Dirt && blk2_is_valid_grass) {
        wrld->worldData[idx] = Block::Grass;
        needsRegen = true;
    }

    if (blk == Block::Sapling) {
        if (is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        } else {
            wrld->make_tree(x, z, y - 1);
            for (int tx = -2; tx < 3; tx++)
                for (int tz = -2; tz < 3; tz++)
                    wrld->update_lighting(x + tx, z + tz);
            needsRegen = true;
        }
    }

    if (blk == Block::Flower1 || blk == Block::Flower2) {
        if (is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        }
    }

    if (blk == Block::Mushroom1 || blk == Block::Mushroom2) {
        int idxl = ((x)*256 * 4) + ((z * 16) * 4) + cY;

        if (!is_dark) {
            wrld->worldData[idx] = Block::Air;
            needsRegen = true;
        }
    }
}

void ChunkMesh::generate(const World *wrld) {

    // Reset + Allocate
    reset_allocate();

    // Loop over the mesh
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 16; y++) {

                int idx = ((x + cX * 16) * 256 * 64) + ((z + cZ * 16) * 64) +
                          (y + cY * 16);

                // Get block
                block_t blk = wrld->worldData[idx];

                // Skip air
                if (blk == 0)
                    continue;

                if (blk == Block::Flower1 || blk == Block::Flower2 ||
                    blk == Block::Mushroom1 || blk == Block::Mushroom2 ||
                    blk == Block::Sapling) {
                    ChunkMeshBuilder::add_xface_to_mesh(
                        this, getTexCoord(blk, LIGHT_TOP), {x, y, z}, LIGHT_TOP,
                        wrld);
                    continue;
                }

                // Update surrounding positions
                SurroundPos surround;
                surround.update(x, y, z);

                // Add 6 faces

                if (blk == Block::Slab) {
                    ChunkMeshBuilder::add_slab_to_mesh(this, wrld, blk,
                                                       {x, y, z}, surround);
                    continue;
                }

                ChunkMeshBuilder::add_block_to_mesh(this, wrld, blk, {x, y, z},
                                                    surround);
            }
        }
    }

    // Finalize the mesh
    finalize_mesh();
}

void ChunkMesh::draw(ChunkMeshSelection meshSel) {
    // Set matrix
    Rendering::RenderContext::get().matrix_translate(
        {cX * 16, cY * 16, cZ * 16});

    // Draw
    meshCollection.select(meshSel)->draw();

    Rendering::RenderContext::get().matrix_clear();
}

} // namespace CrossCraft
