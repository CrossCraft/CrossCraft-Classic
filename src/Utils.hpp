#pragma once
#include <Network/Network.hpp>
#include <Platform/Platform.hpp>
#include <Utilities/Utilities.hpp>
#include <string>

#if BUILD_PLAT != BUILD_VITA
#define PLATFORM_FILE_PREFIX std::string("")
#else
#define PLATFORM_FILE_PREFIX std::string("ux0:/data/CrossCraft-Classic/")
#endif

#if PSP
#include <pspiofilemgr.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace CrossCraft {
using namespace Stardust_Celeste;
inline void netInit() {
#if PSP
    Network::NetworkDriver::get().initGUI();
#elif BUILD_PLAT == BUILD_VITA
    Network::NetworkDriver::get().init();
#endif
}

inline void createDirs() {
#if BUILD_PLAT == BUILD_VITA
    sceIoMkdir("ux0:/data/CrossCraft-Classic", 0777);
    sceIoMkdir("ux0:/data/CrossCraft-Classic/texturepacks", 0777);
    sceIoMkdir("ux0:/data/CrossCraft-Classic/mods", 0777);
    Utilities::Logger::get_app_log()->flush_output = true;
    {
        std::ifstream src("app0:/texturepacks/default.zip", std::ios::binary);
        std::ofstream dst(
            "ux0:/data/CrossCraft-Classic/texturepacks/default.zip",
            std::ios::binary);
        dst << src.rdbuf();
    }
#elif BUILD_PLAT == BUILD_PSP
    sceIoMkdir("./texturepacks", 0777);
    sceIoMkdir("./mods", 0777);
    {
        std::ifstream src("./default.zip", std::ios::binary);
        if (src.is_open()) {
            std::ofstream dst("./texturepacks/default.zip", std::ios::binary);
            dst << src.rdbuf();
            src.close();
            sceIoRemove("./default.zip");
        }
    }
#endif
}
} // namespace CrossCraft
