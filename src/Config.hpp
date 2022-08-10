#pragma once
#include "Utils.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace CrossCraft {

struct Config {
    float sense;
    bool compat;
    std::string ip;
    std::string username;
    u16 port;
    std::string key;

    inline static auto loadConfig() -> Config {
        // Store default values
        Config config;
        config.sense = 1.50f;
        config.compat = 0;
        config.ip = "classic.crosscraft.wtf";
        config.port = 25565;
        config.username = "CC-User";

        std::ifstream file(PLATFORM_FILE_PREFIX + "config.cfg");

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
                } else if (line == "port") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.port;
                } else if (line == "key") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> config.key;
                }
            }
        } else {
            std::ofstream file2(PLATFORM_FILE_PREFIX + "config.cfg");

            file2 << "sense:1.50" << std::endl;
            file2 << "compat:0" << std::endl;
            file2 << "ip:classic.crosscraft.wtf" << std::endl;
            file2 << "port:25565" << std::endl;
            file2 << "username:CC-User" << std::endl;
            file2.close();
        }

        return config;
    }
};

} // namespace CrossCraft
