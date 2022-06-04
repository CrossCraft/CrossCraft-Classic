#pragma once
#include <fstream>
#include <string>
#include <sstream>

namespace CrossCraft {

struct Config {
    float sense;

    inline static auto loadConfig() -> Config{
        Config config;
        config.sense = 1.0f;

        std::ifstream file("config.cfg");

        if (file.is_open()) {
            std::string line;
            std::getline(file, line, ':');

            if (line == "sense") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> config.sense;
            }

        }
        else {
            std::ofstream file2("config.cfg");
            file2 << "sense:1.50" << std::endl;
            file2.close();
        }

        return config;
    }
};

} // namespace CrossCraft