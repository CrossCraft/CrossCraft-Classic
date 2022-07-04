#pragma once
#include <Rendering/Rendering.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace CrossCraft
{

    class TexturePackManager
    {
    public:
        TexturePackManager();
        ~TexturePackManager();

        auto scan_folder(std::string path) -> void;
        auto load_texture(std::string filename, u32 magFilter,
            u32 minFilter, bool repeat, bool flip = false) ->u32;
        auto add_layer(std::string name) -> void;

        inline static auto get() -> TexturePackManager& {
            static TexturePackManager txm;
            return txm;
        }

    private:
        auto extract_zip(std::string path) -> int;

        std::vector<std::string> path_names;
        std::vector<std::string> layers;
    };

}