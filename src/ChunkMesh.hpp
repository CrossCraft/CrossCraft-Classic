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
#include "World.hpp"
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
    void draw();

    /**
     * @brief Draw the transparency layer
     *
     */
    void draw_transparent();

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

    /**
     * @brief Attempt to add a face to the mesh
     *
     */
    void try_add_face(const World *wrld, std::array<float, 12> data,
                      uint8_t blk, glm::vec3 pos, glm::vec3 posCheck,
                      uint32_t lightVal);

    /**
     * @brief Add a face to the mesh
     *
     */
    void add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          glm::vec3 pos, uint32_t lightVal, bool trans);

    int cX, cY, cZ;

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;

    uint16_t tidx_counter;
    std::vector<Rendering::Vertex> t_verts;
    std::vector<uint16_t> t_index;

    Rendering::Mesh mesh;
    Rendering::Mesh transMesh;
};

} // namespace CrossCraft