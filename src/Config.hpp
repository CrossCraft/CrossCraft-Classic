#pragma once
#include <Platform/Platform.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace CrossCraft {

struct Config {
    float sense = 1.50f;
    bool compat = true;
    std::string ip = "192.168.184.130";
    std::string username = "CCC-Client";

    inline static auto loadConfig() -> Config {
        Config config;
        config.sense = 1.0f;

#if BUILD_PLAT != BUILD_VITA
        std::ifstream file("config.cfg");
#else
        std::ifstream file("ux0:/data/CrossCraft-Classic/config.cfg");
#endif
        if (file.is_open()) {
            std::string line;

            while (std::getline(file, line, ':')) {

                if (line == "sense") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.sense;
                } else if (line == "compat") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.compat;
                } else if (line == "ip") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.ip;
                } else if (line == "username") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.username;
                }
            }
        } else {
#if BUILD_PLAT != BUILD_VITA
            std::ofstream file2("config.cfg");
#else
            std::ofstream file2("ux0:/data/CrossCraft-Classic/config.cfg");
#endif
            file2 << "sense:1.50" << std::endl;
            file2 << "compat:0" << std::endl;
            file2 << "client:0" << std::endl;
            file2 << "ip:127.0.0.1" << std::endl;
            file2 << "username:CCC-Client" << std::endl;
            file2.close();
        }

        return config;
    }
};

} // namespace CrossCraft