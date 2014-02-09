#include "lua/Lua.h"
#include <lua.hpp>
#include <lauxlib.h>
#include <sstream>

Lua::Lua() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

Lua::~Lua() {
    lua_close(L);
}

void Lua::doFile(const std::string &filename) {
    LuaStackFrame frame{L};

    const char *name = filename.c_str();
    checkLoadResult(luaL_loadfile(L, name), name);
    pcallWrapper(0, 0, name);
}

void Lua::doString(const std::string &code) {
    LuaStackFrame frame{L};

    static const char *name = "lua string literal";
    checkLoadResult(luaL_loadstring(L, code.c_str()), name);
    pcallWrapper(0, 0, name);
}

void Lua::checkLoadResult(int result, const char *name) {
    if (result == 0) {
        return;
    }

    std::stringstream buf;
    if (result == LUA_ERRFILE) {
        buf << "Failed to open " << name;
    } else if (result == LUA_ERRSYNTAX) {
        buf << "Syntax error in " << name;
        buf << std::endl << lua_tostring(L, -1);
    } else {
        buf << "Out of memory or other error while loading " << name;
    }
    throw LuaException(buf.str().c_str());
}

void Lua::pcallWrapper(int nargs, int nresults, const char *name) {
    int result = lua_pcall(L, nargs, nresults, 0);
    if (result != 0) {
        std::stringstream buf;
        if (result == LUA_ERRRUN) {
            buf << "Error while calling " << name << std::endl << lua_tostring(L, -1);
        } else {
            buf << "Out of memory or unknown error while calling " << name;
        }
        throw LuaException(buf.str().c_str());
    }
}
