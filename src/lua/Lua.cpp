#include "lua/Lua.h"
#include <lua.hpp>
#include <lauxlib.h>
#include <stdexcept>

Lua::Lua() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

Lua::~Lua() {
    lua_close(L);
}

void Lua::doFile(const std::string &filename) {
    if (luaL_loadfile(L, filename.c_str())) {
        throw std::runtime_error("Failed to open lua script");
    }

    if (lua_pcall(L, 0, 0, 0)) {
        throw std::runtime_error("Failed to run lua script");
    }
}

void Lua::call(const std::string &funcname) {
    lua_getglobal(L, funcname.c_str());
    if (lua_isnil(L, -1)) {
        throw LuaException("No function called " + funcname);
    }
    
    if (lua_pcall(L, 0, 0, 0) != 0) {
        throw LuaException("Error running function " + funcname);
    }
}
