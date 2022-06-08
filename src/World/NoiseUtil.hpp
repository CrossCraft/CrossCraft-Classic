#pragma once
#include <FastNoiseLite.h>
#include <cstdint>

namespace CrossCraft::NoiseUtil {

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

auto initialize(int seed = -1) -> void;

/**
 * @brief Remap floats into a range
 *
 * @param input To be remapped
 * @param curr_range_min Current Min
 * @param curr_range_max Current Max
 * @param range_min New Range Min
 * @param range_max New Range Max
 */
inline auto range_map(float &input, float curr_range_min, float curr_range_max,
                      float range_min, float range_max) -> void {
    input = (input - curr_range_min) * (range_max - range_min) /
                (curr_range_max - curr_range_min) +
            range_min;
}

/**
 * @brief Get noise from a position and settings
 *
 * @param x X position
 * @param y Y position
 * @param settings Noise profile
 * @return float
 */
auto get_noise(float x, float y, NoiseSettings *settings) -> float;

extern FastNoiseLite fsl;
extern uint32_t seed;
} // namespace CrossCraft::NoiseUtil
