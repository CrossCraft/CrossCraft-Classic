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

inline auto validate_ivec3(glm::ivec3 ivec) -> bool {
    return ivec.x >= 0 && ivec.x < 256 && ivec.y >= 0 && ivec.y < 256 &&
           ivec.z >= 0 && ivec.z < 256;
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
    auto getIdx(int x, int y, int z) -> uint32_t;

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

  private:
    /**
     * @brief Get the needed chunks
     *
     * @return std::vector<glm::ivec2>
     */
    auto get_needed_chunks() -> std::vector<glm::ivec2>;

    /**
     * @brief Generates a tree given X, Z, and the hash
     *
     */
    auto generate_tree(int x, int z, int hash) -> void;

    glm::ivec2 pchunk_pos;

    ScopePtr<ParticleSystem> psystem;
    unsigned int terrain_atlas;
    float *hmap;
    float tick_counter;

    float place_icd, break_icd;

    friend class DigAction;
    friend class PlaceAction;
    friend class CrossCraftGenerator;
};

} // namespace CrossCraft