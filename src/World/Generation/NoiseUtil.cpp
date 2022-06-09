#include "NoiseUtil.hpp"
#include <ctime>

namespace CrossCraft::NoiseUtil {

FastNoiseLite fsl;
uint32_t seed;

auto initialize(int s) -> void {
    fsl.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    fsl.SetFrequency(0.001f * 5.f);

    if (s == -1)
        s = time(NULL);
    fsl.SetSeed(s);
    seed = s;
}

auto get_noise(float x, float y, NoiseSettings *settings) -> float {

    float amp = settings->amplitude;
    float freq = settings->frequency;

    float sum_noise = 0.0f;
    float sum_amp = 0.0f;

    // Create octaves
    for (auto i = 0; i < settings->octaves; i++) {
        auto noise = fsl.GetNoise(x * freq, y * freq);

        noise *= amp;
        sum_noise += noise;
        sum_amp += amp;

        amp *= settings->persistence;
        freq *= settings->mod_freq;
    }

    // Reset range
    auto divided = sum_noise / sum_amp;

    // Map to the new range;
    range_map(divided, -1.0f, 1.0f, settings->range_min, settings->range_max);

    return divided;
}

} // namespace CrossCraft::NoiseUtil