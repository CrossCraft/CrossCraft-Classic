/**
 * @file ChunkStack.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Chunk Stack object
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "ChunkMesh.hpp"
#include "World.hpp"
#include <array>
#include <glm.hpp>
namespace CrossCraft {

class World;
class ChunkMesh;

/**
 * @brief Chunk Stack
 *
 */
class ChunkStack {
  public:
    /**
     * @brief Construct a new Chunk Stack object
     *
     * @param x X Position
     * @param z Y Position
     */
    ChunkStack(int x, int z);

    /**
     * @brief Destroy the Chunk Stack object
     *
     */
    ~ChunkStack();

    /**
     * @brief Generate the stack
     *
     * @param wrld The world to reference
     */
    void generate(World *wrld);

    /**
     * @brief Update Chunk
     *
     * @param wrld The world to reference
     */
    void chunk_update(World *wrld);

    /**
     * @brief Random Tick Update
     *
     * @param wrld The world to reference
     */
    void rtick_update(World *wrld);

    /**
     * @brief Draw the chunk stack
     *
     */
    void draw();

    /**
     * @brief Draw the transparent chunks
     *
     */
    void draw_transparent();

    /**
     * @brief Get the chunk position
     *
     * @return glm::ivec2
     */
    inline auto get_chunk_pos() -> glm::ivec2 { return {cX, cZ}; }

  private:
    std::array<ChunkMesh *, 4> stack;
    int cX, cZ;
};

} // namespace CrossCraft