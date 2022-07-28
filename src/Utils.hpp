#pragma once
#include <Utilities/Utilities.hpp>
#include <Platform/Platform.hpp>
#include <Network/Network.hpp>
#include <string>

#if BUILD_PLAT != BUILD_VITA
#define PLATFORM_FILE_PREFIX std::string("")
#else
#define PLATFORM_FILE_PREFIX std::string("ux0:/data/CrossCraft-Classic/")
#endif

namespace CrossCraft
{
    inline void netInit()
    {
#if PSP
        Network::NetworkDriver::get().initGUI();
#elif BUILD_PLAT == BUILD_VITA
        Network::NetworkDriver::get().init();
#endif
    }

    inline void createDirs()
    {
#if BUILD_PLAT == BUILD_VITA
        sceIoMkdir("ux0:/data/CrossCraft-Classic", 0777);
        sceIoMkdir("ux0:/data/CrossCraft-Classic/texturepacks", 0777);

        {
            std::ifstream src("app0:/texturepacks/default.zip", std::ios::binary);
            std::ofstream dst(
                "ux0:/data/CrossCraft-Classic/texturepacks/default.zip",
                std::ios::binary);
            dst << src.rdbuf();
        }
#endif
    }
}