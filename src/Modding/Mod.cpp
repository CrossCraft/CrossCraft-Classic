#include "Mod.hpp"
#include "../Utils.hpp"
#include "../World/World.hpp"
#include <filesystem>

namespace CrossCraft::Modding {

World *ModManager::wrld = nullptr;

Mod::Mod(std::string path) {
    L = luaL_newstate();
    luaL_openlibs(L);
    ModManager::init_lib(L);

    std::ifstream file(path + "mod.txt");

    hasStart = false;
    hasUpdate = false;
    hasTick = false;
    hasPlace = false;
    hasBreak = false;

    if (file.is_open()) {
        std::string line;

        while (std::getline(file, line, ':')) {
            if (line == "hasStart") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> hasStart;
            } else if (line == "hasUpdate") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> hasUpdate;
            } else if (line == "hasTick") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> hasTick;
            } else if (line == "hasPlace") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> hasPlace;
            } else if (line == "hasBreak") {
                std::getline(file, line);
                std::stringstream str(line);

                str >> hasBreak;
            }
        }
    }

    ok = true;
    if (luaL_loadfile(L, (path + "script.lua").c_str())) {
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
        SC_APP_INFO("FAILED LOADING MOD AT {}", path);
        ok = false;
        return;
    }

    if (lua_pcall(L, 0, 0, 0))
        fprintf(stderr, "lua_pcall() failed");
}
Mod::~Mod() { lua_close(L); }

void Mod::onStart() {
    if (!hasStart)
        return;

    lua_getglobal(L, "onStart");

    if (lua_pcall(L, 0, 0, 0) != 0) {
        fprintf(stderr, "error running function `onStart`': %s\n",
                lua_tostring(L, -1));
    }
}
void Mod::onUpdate() {
    if (!hasUpdate)
        return;

    lua_getglobal(L, "onUpdate");

    if (lua_pcall(L, 0, 0, 0) != 0) {
        fprintf(stderr, "error running function `onUpdate`': %s\n",
                lua_tostring(L, -1));
    }
}
void Mod::onTick() {
    if (!hasTick)
        return;

    lua_getglobal(L, "onTick");

    if (lua_pcall(L, 0, 0, 0) != 0) {
        fprintf(stderr, "error running function `onTick`': %s\n",
                lua_tostring(L, -1));
    }
}
void Mod::onPlace(glm::vec3 location, uint8_t type) {
    if (!hasPlace)
        return;

    lua_getglobal(L, "onPlace");
    lua_pushnumber(L, location.x);
    lua_pushnumber(L, location.y);
    lua_pushnumber(L, location.z);
    lua_pushinteger(L, type);
    if (lua_pcall(L, 4, 0, 0) != 0) {
        fprintf(stderr, "error running function `onPlace`': %s\n",
                lua_tostring(L, -1));
    }
}
void Mod::onBreak(glm::vec3 location, uint8_t type) {
    if (!hasBreak)
        return;

    lua_getglobal(L, "onBreak");
    lua_pushnumber(L, location.x);
    lua_pushnumber(L, location.y);
    lua_pushnumber(L, location.z);
    lua_pushinteger(L, type);
    if (lua_pcall(L, 4, 0, 0) != 0) {
        fprintf(stderr, "error running function `onBreak`': %s\n",
                lua_tostring(L, -1));
    }
}

void ModManager::scan_mods() {
    std::filesystem::path p(PLATFORM_FILE_PREFIX + "mods/");

    for (const auto &entry : std::filesystem::directory_iterator(p)) {
        if (entry.is_directory()) {
            auto p = PLATFORM_FILE_PREFIX + "mods/" +
                     entry.path().filename().string() + "/";
            SC_APP_INFO("Found Mod Candidate {}", p);
            ScopePtr<Mod> m = create_scopeptr<Mod>(p);

            if (m->ok) {
                mods.push_back(std::move(m));
            }
        }
    }
}

static int lua_world_get(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 3)
        return luaL_error(L, "Error: World.getBlk() takes 3 arguments.");

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int z = luaL_checkinteger(L, 3);

    auto blk = ModManager::wrld->worldData[ModManager::wrld->getIdx(x, y, z)];

    lua_pushinteger(L, blk);
    return 1;
}

static int lua_world_set(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 4)
        return luaL_error(L, "Error: World.setBlk() takes 4 arguments.");

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int z = luaL_checkinteger(L, 3);
    int blk = luaL_checkinteger(L, 4);

    ModManager::wrld->worldData[ModManager::wrld->getIdx(x, y, z)] = blk;
    ModManager::wrld->update_lighting(x, z);

    uint16_t cx = x / 16;
    uint16_t cy = z / 16;
    uint32_t id = cx << 16 | (cy & 0x00FF);

    if (ModManager::wrld->chunks.find(id) != ModManager::wrld->chunks.end())
        ModManager::wrld->chunks[id]->generate(ModManager::wrld);

    ModManager::wrld->update_surroundings(x, z);
    ModManager::wrld->update_nearby_blocks({x, y, z});

    return 0;
}

static int lua_player_getX(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 0)
        return luaL_error(L, "Error: Player.getX() takes 0 arguments.");

    lua_pushinteger(L, ModManager::wrld->player->pos.x);
    return 1;
}

static int lua_player_getY(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 0)
        return luaL_error(L, "Error: Player.getY() takes 0 arguments.");

    lua_pushinteger(L, ModManager::wrld->player->pos.y);
    return 1;
}

static int lua_player_getZ(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 0)
        return luaL_error(L, "Error: Player.getZ() takes 0 arguments.");

    lua_pushinteger(L, ModManager::wrld->player->pos.z);
    return 1;
}

static int lua_player_setX(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 1)
        return luaL_error(L, "Error: Player.setX() takes 1 arguments.");

    float f = luaL_checkinteger(L, 1);
    ModManager::wrld->player->pos.x = f;

    return 0;
}

static int lua_player_setY(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 1)
        return luaL_error(L, "Error: Player.setY() takes 1 arguments.");

    float f = luaL_checkinteger(L, 1);
    ModManager::wrld->player->pos.y = f;

    return 0;
}

static int lua_player_setZ(lua_State *L) {
    int argc = lua_gettop(L);
    if (argc != 1)
        return luaL_error(L, "Error: Player.setZ() takes 1 arguments.");

    float f = luaL_checkinteger(L, 1);
    ModManager::wrld->player->pos.z = f;

    return 0;
}

static const luaL_Reg worldLib[] = {
    {"getBlk", lua_world_get}, {"setBlk", lua_world_set}, {0, 0}};

static const luaL_Reg playerLib[] = {{"getX", lua_player_getX},
                                     {"setX", lua_player_setX},
                                     {"getY", lua_player_getY},
                                     {"setY", lua_player_setY},
                                     {"getZ", lua_player_getZ},
                                     {"setZ", lua_player_setZ},
                                     {0, 0}};

auto ModManager::init_lib(lua_State *L) -> void {
    lua_getglobal(L, "World");

    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }

    luaL_setfuncs(L, worldLib, 0);
    lua_setglobal(L, "World");

    lua_getglobal(L, "Player");

    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }

    luaL_setfuncs(L, playerLib, 0);
    lua_setglobal(L, "Player");
}

} // namespace CrossCraft::Modding