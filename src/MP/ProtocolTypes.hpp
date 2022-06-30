#pragma once
#include <cstdint>

const unsigned int STRING_LENGTH = 64;
const unsigned int ARRAY_LENGTH = 1024;

typedef uint8_t Byte;
typedef int8_t SByte;
typedef int16_t Short;

struct String
{
    Byte contents[STRING_LENGTH];
};

struct ByteArray
{
    Byte contents[ARRAY_LENGTH];
};

struct BasePacket
{
    virtual ~BasePacket() = default;

    Byte PacketID;
};
