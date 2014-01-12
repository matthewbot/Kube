#ifndef CALL_H
#define CALL_H

#include "lua/bindings.h"
#include "util/templates.h"

#include <lua.hpp>

namespace detail {
    int traceback(lua_State *L);
    
    template <typename Callable>
    struct CallLuaHelper { };

    template <typename... Args>
    struct CallLuaHelper<void (*)(Args...)> {
        template <typename... Ts>
        static void call(lua_State *L, const std::string &name, Ts&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Ts), "Incorrect number of arguments");

            lua_pushcfunction(L, traceback);
            int errfunc = lua_gettop(L);
            lua_getglobal(L, name.c_str());
            pushAllCType(L, TypeSequence<Args...>(), std::forward<Ts>(args)...);
            if (lua_pcall(L, sizeof...(Args), 0, errfunc) != 0) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
    };
    
    template <typename Ret, typename... Args>
    struct CallLuaHelper<Ret (*)(Args...)> {
        template <typename... Ts>
        static Ret call(lua_State *L, const std::string &name, Ts&&... args) {
            static_assert(sizeof...(Args) == sizeof...(Ts), "Incorrect number of arguments");

            lua_pushcfunction(L, traceback);
            int errfunc = lua_gettop(L);
            lua_getglobal(L, name.c_str());
            pushAllCType(L, TypeSequence<Args...>(), std::forward<Ts>(args)...);
            if (lua_pcall(L, sizeof...(Args), 1, errfunc) != 0) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            Ret val = toCType<Ret>(L, -1);
            lua_pop(L, 2);
            return val;
        }
    };
}
    
template <typename Callable, typename... Ts>
auto callLua(lua_State *L, const std::string &name, Ts&&... args)
-> decltype(detail::CallLuaHelper<Callable *>::call(L, name, std::forward<Ts>(args)...)) {
    return detail::CallLuaHelper<Callable *>::call(L, name, std::forward<Ts>(args)...);
}

#endif
