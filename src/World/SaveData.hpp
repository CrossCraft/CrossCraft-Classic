#pragma once
#include <any>
#include "World.hpp"

namespace CrossCraft
{
    class World;

    class SaveData
    {
    public:
        static auto load_world(World *wrld) -> bool;
        static auto save(std::any p) -> void;
    };
} // namespace CrossCraft
