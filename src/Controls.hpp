#pragma once
#include "Utils.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#if !(BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA)
#include <GLFW/glfw3.h>

namespace GI {
extern GLFWwindow *window;
}

static int glfwretval = 0;
static void controls_key_callback(GLFWwindow *window, int key, int scancode,
                                  int action, int mods) {
    glfwretval = key;
}

#endif

namespace CrossCraft {

struct Controls {

    Controls() { loadControls(); }

    // PSP Analog View (true) vs Analog Move
    bool pspJoystickView;

    // PS Vita Left Analog View (true) vs Right Analog View
    bool vitaJoystickSwap;

    // PS2 Left Analog View (true) vs Right Analog View
    bool ps2JoystickSwap;

    // Keyboard keys

    int keyForward;
    int keyBack;
    int keyLeft;
    int keyRight;

    int keyJump;
    int keyChat;
    int keyRespawn;
    int keyTab;

    inline auto getNextKey() -> int {
#if !(BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA)
        glfwretval = 0;
        auto prev = glfwSetKeyCallback(GI::window, controls_key_callback);
        Stardust_Celeste::delay(1000);
        glfwPollEvents();
        glfwSetKeyCallback(GI::window, prev);

        return glfwretval;
#endif
        return 0;
    }

    inline auto getKeyName(int key) -> std::string {
#if !(BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA)
        auto res = glfwGetKeyName(key, 0);
        if (res != nullptr) {
            return std::string(res);
        } else {

            switch (key) {
            case GLFW_KEY_SPACE:
                return "space";

            case GLFW_KEY_LEFT_SHIFT:
                return "lshift";

            case GLFW_KEY_RIGHT_SHIFT:
                return "rshift";

            case GLFW_KEY_TAB:
                return "tab";

            case GLFW_KEY_ESCAPE:
                return "escape";

            default:
                return "";
            }
        }
#else
        return "";
#endif
    }

    inline auto upperKeyName(int key) -> std::string {
        auto str = getKeyName(key);
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        return str;
    }

    inline auto loadControls() -> void { // Store default values
        pspJoystickView = true;
        vitaJoystickSwap = true;
        ps2JoystickSwap = true;

#if !(BUILD_PLAT == BUILD_PSP || BUILD_PLAT == BUILD_VITA)
        keyForward = GLFW_KEY_W;
        keyBack = GLFW_KEY_S;
        keyLeft = GLFW_KEY_A;
        keyRight = GLFW_KEY_D;

        keyJump = GLFW_KEY_SPACE;
        keyChat = GLFW_KEY_T;
        keyRespawn = GLFW_KEY_R;
        keyTab = GLFW_KEY_TAB;
#endif

        std::ifstream file(PLATFORM_FILE_PREFIX + "controls.txt");

        if (file.is_open()) {
            std::string line;

            while (std::getline(file, line, ':')) {

                if (line == "pspJoystick") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> pspJoystickView;
                } else if (line == "vitaJoystick") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> vitaJoystickSwap;
                } else if (line == "ps2Joystick") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> ps2JoystickSwap;
                } else if (line == "keyForward") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyForward;
                } else if (line == "keyBack") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyBack;
                } else if (line == "keyLeft") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyLeft;
                } else if (line == "keyRight") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyRight;
                } else if (line == "keyJump") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyJump;
                } else if (line == "keyChat") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyChat;
                } else if (line == "keyRespawn") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyRespawn;
                } else if (line == "keyTab") {
                    std::getline(file, line);
                    std::stringstream str(line);

                    str >> keyTab;
                }
            }
        } else {
            writeControls();
        }
    }

    inline auto writeControls() -> void {
        std::ofstream file2(PLATFORM_FILE_PREFIX + "controls.txt");

        file2 << "pspJoystick:" << pspJoystickView << std::endl;
        file2 << "ps2Joystick:" << ps2JoystickSwap << std::endl;
        file2 << "vitaJoystick:" << vitaJoystickSwap << std::endl;
        file2 << "keyForward:" << keyForward << std::endl;
        file2 << "keyBack:" << keyBack << std::endl;
        file2 << "keyLeft:" << keyLeft << std::endl;
        file2 << "keyRight:" << keyRight << std::endl;
        file2 << "keyJump:" << keyJump << std::endl;
        file2 << "keyChat:" << keyChat << std::endl;
        file2 << "keyTab:" << keyTab << std::endl;
        file2 << "keyRespawn:" << keyRespawn << std::endl;

        file2.close();
    }

    inline static auto get() -> Controls & {
        static Controls mm;
        return mm;
    }
};

} // namespace CrossCraft