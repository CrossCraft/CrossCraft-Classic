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
#include "ChunkStack.hpp"
#include "Clouds.hpp"
#include "Config.hpp"
#include "Particles.hpp"
#include "Player.hpp"
#include <FastNoiseLite.h>
#include <Utilities/Types.hpp>
#include <glm.hpp>
#include <map>
#include <memory>
#include <stdint.h>
#include <vector>

namespace CrossCraft {

class Player;

typedef uint8_t block_t;
class ChunkStack;

/**
 * @brief Describes a noise profile for the world generator
 *
 */
struct NoiseSettings {
    uint8_t octaves;
    float amplitude;
    float frequency;
    float persistence;
    float mod_freq;
    float offset;

    float range_min;
    float range_max;
};

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
     * @brief Generate the entire world with perlin noise
     *
     */
    auto generate() -> void;

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

    static auto dig(std::any d) -> void;
    static auto place(std::any d) -> void;

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

    std::map<int, ChunkStack *> chunks;

    auto make_tree(int x, int z, int h) -> void;

  private:
    /**
     * @brief Get noise from a position and settings
     *
     * @param x X position
     * @param y Y position
     * @param settings Noise profile
     * @return float
     */
    auto get_noise(float x, float y, NoiseSettings *settings) -> float;

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
    FastNoiseLite fsl;
    float *hmap;
    uint32_t seed;
    float tick_counter;

    float place_icd, break_icd;
};

} // namespace CrossCraft