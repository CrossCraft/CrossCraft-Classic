#pragma once
#include <Utilities/Singleton.hpp>
#include <Utilities/Utilities.hpp>
#include <glm.hpp>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <string>
#include <vector>

namespace CrossCraft {
class World;
}

namespace CrossCraft::Modding {
using namespace Stardust_Celeste;
class Mod {
  public:
    Mod(std::string path);
    ~Mod();

    void onStart();
    void onUpdate();
    void onTick();
    void onPlace(glm::vec3 location, uint8_t type);
    void onBreak(glm::vec3 location, uint8_t type);

    bool ok;

  private:
    lua_State *L;

    bool hasStart;
    bool hasUpdate;
    bool hasTick;
    bool hasPlace;
    bool hasBreak;
};

class ModManager : public Singleton {
  public:
    inline void onStart() {
        for (auto &m : mods) {
            m->onStart();
        }
    }
    inline void onUpdate() {
        for (auto &m : mods) {
            m->onUpdate();
        }
    }
    inline void onTick() {
        for (auto &m : mods) {
            m->onTick();
        }
    }
    inline void onPlace(glm::vec3 location, uint8_t type) {
        for (auto &m : mods) {
            m->onPlace(location, type);
        }
    }
    inline void onBreak(glm::vec3 location, uint8_t type) {
        for (auto &m : mods) {
            m->onBreak(location, type);
        }
    }

    inline static auto get() -> ModManager & {
        static ModManager mm;
        return mm;
    }

    inline auto get_num_mods() -> int { return mods.size(); }

    inline static auto set_ptr(World *w) -> void { wrld = w; }

    static auto init_lib(lua_State *L) -> void;

    static World *wrld;

  private:
    void scan_mods();
    Mod load_mod(std::string path);

    ModManager() { scan_mods(); }
    std::vector<ScopePtr<Mod>> mods;
};

} // namespace CrossCraft::Modding
