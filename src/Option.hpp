#pragma once
#include "Utils.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace CrossCraft {

struct Option {

    Option() { loadOptions(); }

    bool music;
    bool sound;

    int renderDist;
    bool bobbing;

    bool fps;
    bool vsync;

    inline auto loadOptions() -> void { // Store default values
        music = true;
        sound = true;
        renderDist = 2;
        bobbing = true;
        fps = true;
        vsync = true;

        std::ifstream file(PLATFORM_FILE_PREFIX + "options.txt");

        if (file.is_open()) {
            std::string line;

            while (std::getline(file, line, ':')) {

                if (line == "music") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> music;
                } else if (line == "sound") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> sound;
                } else if (line == "fps") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> fps;
                } else if (line == "vsync") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> vsync;
                } else if (line == "bobbing") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> bobbing;
                } else if (line == "renderDist") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> renderDist;
                }
            }
        } else {
            writeOptions();
        }
    }

    inline auto writeOptions() -> void {
        std::ofstream file2(PLATFORM_FILE_PREFIX + "options.txt");

        file2 << "music:" << (int)music << std::endl;
        file2 << "sound:" << (int)sound << std::endl;
        file2 << "renderDist:" << (int)renderDist << std::endl;
        file2 << "bobbing:1" << (int)bobbing << std::endl;
        file2 << "fps:1" << (int)fps << std::endl;
        file2 << "vsync:1" << (int)vsync << std::endl;
        file2.close();
    }

    inline static auto get() -> Option & {
        static Option mm;
        return mm;
    }
};

} // namespace CrossCraft