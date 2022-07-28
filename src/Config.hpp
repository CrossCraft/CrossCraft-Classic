#pragma once
#include "Utils.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace CrossCraft
{

    struct Config
    {
        float sense;
        bool compat;
        std::string ip;
        std::string username;

        inline static auto loadConfig() -> Config
        {
            Config config;
            config.sense = 1.0f;

            std::ifstream file(PLATFORM_FILE_PREFIX + "config.cfg");

            if (file.is_open())
            {
                std::string line;

                while (std::getline(file, line, ':'))
                {

                    if (line == "sense")
                    {
                        std::getline(file, line);
                        std::stringstream str(line);

                        str >> config.sense;
                    }
                    else if (line == "compat")
                    {
                        std::getline(file, line);
                        std::stringstream str(line);

                        str >> config.compat;
                    }
                    else if (line == "ip")
                    {
                        std::getline(file, line);
                        std::stringstream str(line);

                        str >> config.ip;
                    }
                    else if (line == "username")
                    {
                        std::getline(file, line);
                        std::stringstream str(line);

                        str >> config.username;
                    }
                }
            }
            else
            {
                std::ofstream file2(PLATFORM_FILE_PREFIX + "config.cfg");

                file2 << "sense:1.50" << std::endl;
                file2 << "compat:0" << std::endl;
                file2 << "ip:127.0.0.1" << std::endl;
                file2 << "username:CCC-Client" << std::endl;
                file2.close();
            }

            return config;
        }
    };

} // namespace CrossCraft