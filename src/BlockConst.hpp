#pragma once
#include <array>
#include <cstdint>

const std::array<float, 12> frontFace{
    0,
    0,
    1,
    1,
    0,
    1,
    1,
    1,
    1,
    0,
    1,
    1,
};

const std::array<float, 12> backFace{
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    1,
    1,
    0,
};

const std::array<float, 12> leftFace{
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    1,
    1,
    0,
    1,
    0,
};

const std::array<float, 12> rightFace{
    1,
    0,
    1,
    1,
    0,
    0,
    1,
    1,
    0,
    1,
    1,
    1,
};

const std::array<float, 12> frontFaceHalf{
    0,
    0,
    1,
    1,
    0,
    1,
    1,
    0.5f,
    1,
    0,
    0.5f,
    1,
};

const std::array<float, 12> backFaceHalf{
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0.5f,
    0,
    1,
    0.5f,
    0,
};

const std::array<float, 12> leftFaceHalf{
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    0.5f,
    1,
    0,
    0.5f,
    0,
};

const std::array<float, 12> rightFaceHalf{
    1,
    0,
    1,
    1,
    0,
    0,
    1,
    0.5f,
    0,
    1,
    0.5f,
    1,
};

const std::array<float, 12> topFace{
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    1,
    0,
};

const std::array<float, 12> bottomFace{0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

const std::array<float, 12> xFace1{
    0,
    0,
    0,
    1,
    0,
    1,
    1,
    1,
    1,
    0,
    1,
    0,
};

const std::array<float, 12> xFace2{
    0,
    0,
    1,
    1,
    0,
    0,
    1,
    1,
    0,
    0,
    1,
    1,
};
const std::array<float, 12> xFace3{
    0,
    0,
    0,
    0,
    1,
    0,
    1,
    1,
    1,
    1,
    0,
    1,
};

const std::array<float, 12> xFace4{
    0,
    0,
    1,
    0,
    1,
    1,
    1,
    1,
    0,
    1,
    0,
    0,
};

constexpr uint32_t LIGHT_TOP = 0xFFFFFFFF;
constexpr uint32_t LIGHT_SIDE_X = 0xFF999999;
constexpr uint32_t LIGHT_SIDE_Z = 0xFFCCCCCC;
constexpr uint32_t LIGHT_BOT = 0xFF7F7F7F;

// When dark block color tint is multiplied by 0.6
constexpr uint32_t LIGHT_TOP_DARK = 0xFF999999;
constexpr uint32_t LIGHT_SIDE_X_DARK = 0xFF5C5C5C;
constexpr uint32_t LIGHT_SIDE_Z_DARK = 0xFF7A7A7A;
constexpr uint32_t LIGHT_BOT_DARK = 0xFF4C4C4C;

namespace Block
{
    const uint8_t Air = 0;
    const uint8_t Stone = 1;
    const uint8_t Dirt = 3;
    const uint8_t Grass = 2;
    const uint8_t Cobblestone = 4;
    const uint8_t Wood = 5;
    const uint8_t Sapling = 6;
    const uint8_t Bedrock = 7;
    const uint8_t Water = 8;
    const uint8_t Still_Water = 9;
    const uint8_t Lava = 10;
    const uint8_t Still_Lava = 11;
    const uint8_t Sand = 12;
    const uint8_t Gravel = 13;
    const uint8_t Gold_Ore = 14;
    const uint8_t Iron_Ore = 15;
    const uint8_t Coal_Ore = 16;
    const uint8_t Logs = 17;
    const uint8_t Leaves = 18;
    const uint8_t Sponge = 19;
    const uint8_t Glass = 20;
    const uint8_t Flower1 = 37;
    const uint8_t Flower2 = 38;
    const uint8_t Mushroom1 = 39;
    const uint8_t Mushroom2 = 40;
    const uint8_t Gold = 41;
    const uint8_t Iron = 42;
    const uint8_t Double_Slab = 43;
    const uint8_t Slab = 44;
    const uint8_t Brick = 45;
    const uint8_t TNT = 46;
    const uint8_t Bookshelf = 47;
    const uint8_t Mossy_Rocks = 48;
    const uint8_t Obsidian = 49;
} // namespace Block
