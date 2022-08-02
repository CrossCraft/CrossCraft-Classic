/**
 * @file World.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief The World
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../Chunk/ChunkStack.hpp"
#include "../Config.hpp"
#include "../MP/Client.hpp"
#include "../Player/Player.hpp"
#include "Clouds.hpp"
#include "Particles.hpp"
#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <map>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <vector>
namespace CrossCraft {

struct ChunkMeta {
    bool is_empty;
    bool is_full;
};

inline auto validate_ivec3(glm::ivec3 ivec, glm::vec3 size) -> bool {
    return ivec.x >= 0 && ivec.x < size.x && ivec.y >= 0 && ivec.y < size.y &&
           ivec.z >= 0 && ivec.z < size.z;
}

class Player;

typedef uint8_t block_t;
class ChunkStack;

/**
 * @brief The world
 *
 */
class World {
  public:
    /**
     * @brief Construct a new World object
     *
     * @param p Player to use
     */
    World(RefPtr<Player> p);

    /**
     * @brief Destroy the World object
     *
     */
    ~World();

    auto load_world() -> bool;

    /**
     * @brief Update (checks chunks, run chunk updates, tick updates);
     *
     * @param dt Delta Time
     */
    auto update(double dt) -> void;

    /**
     * @brief Draw the world
     *
     */
    auto draw() -> void;

    /**
     * @brief Get a World Index
     *
     * @param x Position
     * @param y Position
     * @param z Position
     * @return uint32_t block_t* worldData
     */
    auto getIdx(int x, int y, int z) -> uint32_t const;

    /**
     * @brief Get a World Light Index
     *
     * @param x Position
     * @param y Position
     * @param z Position
     * @return uint32_t block_t* worldData
     */
    auto getIdxl(int x, int y, int z) -> uint32_t const;

    block_t *worldData;
    uint16_t *lightData;
    RefPtr<Player> player;
    ScopePtr<Clouds> clouds;
    Config cfg;

    /**
     * @brief Updates surrounding chunks
     *
     * @param x X of block modified
     * @param z Z of block modified
     */
    auto update_surroundings(int x, int z) -> void;

    /**
     * @brief Updates lighting
     *
     * @param x X of block mod
     * @param z Z of block mod
     */
    auto update_lighting(int x, int z) -> void;

    auto generate_meta() -> void;

    /**
     * @brief Updates nearby block states
     *
     * @param ivec Block source from update
     */
    auto update_nearby_blocks(glm::ivec3 ivec) -> void;

    /**
     * @brief Calculate and add update to chunk
     *
     */
    auto add_update(glm::ivec3 ivec) -> void;

    auto spawn() -> void;
    static auto save(std::any p) -> void;

    std::map<int, ChunkStack *> chunks;
    ChunkMeta *chunksMeta;

    auto set_block(short x, short y, short z, uint8_t mode, uint8_t block)
        -> void;

    MP::Client *client;

    glm::vec3 world_size;

  private:
    /**
     * @brief Get the needed chunks
     *
     * @return std::vector<glm::ivec2>
     */
    auto get_needed_chunks() -> std::vector<glm::ivec2>;

    glm::ivec2 pchunk_pos;

    ScopePtr<ParticleSystem> psystem;
    unsigned int terrain_atlas;
    float *hmap;
    float tick_counter;

    float place_icd, break_icd;
    float chunk_generate_icd;

    std::map<float, glm::ivec2> to_generate;

    uint16_t idx_counter;
    std::vector<Rendering::Vertex> m_verts;
    std::vector<uint16_t> m_index;
    Rendering::Mesh blockMesh;

    auto add_face_to_mesh(std::array<float, 12> data, std::array<float, 8> uv,
                          uint32_t lightVal, glm::vec3 mypos) -> void;

    auto draw_selection() -> void;

    friend class DigAction;
    friend class PlaceAction;
    friend class CrossCraftGenerator;
    friend class ClassicGenerator;
};

} // namespace CrossCraft