/**
 * @file ChunkMesh.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Describes a 16x16x16 chunk mesh
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../World/World.hpp"
#include "ChunkUtil.hpp"
#include <Rendering/Rendering.hpp>

using namespace Stardust_Celeste;

namespace CrossCraft {

class World;

/**
 * @brief Chunk mesh object.
 */
class ChunkMesh {
  public:
    /**
     * @brief Construct a new Chunk Mesh object at a location
     *
     * @param x Chunk X position
     * @param y Chunk Y position
     * @param z Chunk Z position
     */
    ChunkMesh(int x, int y, int z);

    /**
     * @brief Destroy the Chunk Mesh object
     */
    ~ChunkMesh();

    /**
     * @brief Generate a brand new mesh
     *
     * @param wrld - World to reference
     */
    void generate(const World *wrld);

    /**
     * @brief Draw the chunk mesh
     *
     */
    void draw(ChunkMeshSelection meshSel);

    /**
     * @brief Random Ticks the section
     *
     * @param wrld
     */
    void rtick(World *wrld);

    bool needsRegen;

  private:
    /**
     * @brief Reset a mesh and allocate space
     *
     */
    void reset_allocate();

    /**
     * @brief Finalize a mesh - cleanup memory use
     *
     */
    void finalize_mesh();

    int cX, cY, cZ;
    int rtcounter;

    ChunkMeshCollection meshCollection;

    friend class ChunkMeshBuilder;
};

} // namespace CrossCraft