#pragma once
#include <array>

const std::array<float, 12> frontFace{
    0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1,
};

const std::array<float, 12> backFace{
    1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
};

const std::array<float, 12> leftFace{
    0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,
};

const std::array<float, 12> rightFace{
    1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
};

const std::array<float, 12> frontFaceHalf{
    0, 0, 1, 1, 0, 1, 1, 0.5f, 1, 0, 0.5f, 1,
};

const std::array<float, 12> backFaceHalf{
    1, 0, 0, 0, 0, 0, 0, 0.5f, 0, 1, 0.5f, 0,
};

const std::array<float, 12> leftFaceHalf{
    0, 0, 0, 0, 0, 1, 0, 0.5f, 1, 0, 0.5f, 0,
};

const std::array<float, 12> rightFaceHalf{
    1, 0, 1, 1, 0, 0, 1, 0.5f, 0, 1, 0.5f, 1,
};

const std::array<float, 12> topFace{
    0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0,
};

const std::array<float, 12> bottomFace{0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

const std::array<float, 12> xFace1{
    0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0,
};

const std::array<float, 12> xFace2{
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
};
const std::array<float, 12> xFace3{
    0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1,
};

const std::array<float, 12> xFace4{
    0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0,
};

constexpr uint32_t LIGHT_TOP = 0xFFFFFFFF;
constexpr uint32_t LIGHT_SIDE = 0xFFCCCCCC;
constexpr uint32_t LIGHT_BOT = 0xFF999999;

constexpr uint32_t LIGHT_TOP_DARK = 0xFFDDDDDD;
constexpr uint32_t LIGHT_SIDE_DARK = 0xFFAAAAAA;
constexpr uint32_t LIGHT_BOT_DARK = 0xFF777777;
