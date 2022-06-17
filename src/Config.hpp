#pragma once
#include <fstream>
#include <sstream>
#include <string>

namespace CrossCraft {

struct Config {
    float sense;
    bool compat;

    inline static auto loadConfig() -> Config {
        Config config;
        config.sense = 1.0f;

        std::ifstream file("config.cfg");

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
                }
            }

        } else {
            std::ofstream file2("config.cfg");
            file2 << "sense:1.50" << std::endl;
            file2 << "compat:0" << std::endl;
            file2.close();
        }

        return config;
    }
};

} // namespace CrossCraft