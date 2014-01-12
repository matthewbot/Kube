#include "MetatableBuilder.h"

namespace detail {
    // TODO writing these in Lua almost certainly faster
    int metaIndex(lua_State *L) {
        // Check for getter function
        if (lua_isstring(L, 2)) {
            std::string key = lua_tostring(L, 2);
            lua_pushstring(L, ("get_" + key).c_str());
            lua_gettable(L, lua_upvalueindex(1));
            if (!lua_isnil(L, -1)) {
                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                return 1;
            }
        }

        // Check for method
        lua_pushvalue(L, 2);
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            return 1;
        }

        // Check for index method
        lua_pushstring(L, "index");
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            return 1;
        }

        return 1;
    }

    int metaNewIndex(lua_State *L) {
        if (lua_isstring(L, 2)) {
            std::string key = lua_tostring(L, 2);
            lua_pushstring(L, ("set_" + key).c_str());
            lua_gettable(L, lua_upvalueindex(1));
            if (!lua_isnil(L, -1)) {
                lua_pushvalue(L, 1);
                lua_pushvalue(L, 3);
                lua_call(L, 2, 0);
                return 0;
            }
        }

        lua_pushstring(L, "newindex");
        lua_gettable(L, lua_upvalueindex(1));
        if (!lua_isnil(L, -1)) {
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_call(L, 3, 0);
            return 0;
        }

        lua_pushstring(L, "Attempt to write to unknown field");
        return lua_error(L);
    }
}
