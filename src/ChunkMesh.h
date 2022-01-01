#pragma once
#include "World.h"
#include <GFX/GFXWrapper.h>

using namespace Stardust;

class World;

/**
 * Chunk mesh object.
 */
class ChunkMesh {
  public:
    /**
     * Creates a mesh.
     *
     * \param x - Chunk X
     * \param y - Chunk Y
     * \param z - Chunk Z
     */
    ChunkMesh(int x, int y, int z);
    ~ChunkMesh();

    /**
     * Generates a mesh. If it already exists, then it will be recreated.
     *
     * \param wrld - World to get data from
     */
    void generate(const World *wrld);

    /**
     * Draws to screen assuming atlas is bound.
     *
     */
    void draw();

    /**
     * Draws transparent objects after.
     */
    void drawTransparent();

  private:
    void tryAddFace(const World *wrld, std::array<float, 12> data, uint8_t blk,
                    glm::vec3 pos, glm::vec3 posCheck, float lightVal);
    void addFaceToMesh(std::array<float, 12> data, std::array<float, 8> uv,
                       glm::vec3 pos, float lightVal, bool trans);
    GFX::Mesh mesh;
    GFX::Model model;

    GFX::Mesh tmesh;
    GFX::Model tmodel;

    int cX, cY, cZ;
    unsigned int idx_counter;
    unsigned int tidx_counter;
};