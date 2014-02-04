#ifndef BINDINGS_H
#define BINDINGS_H

#include "lua/UserData.h"
#include "util/templates.h"

#include <lua.hpp>
#include <typeinfo>
#include <type_traits>
#include <memory>
#include <utility>

namespace detail {
    template <typename T, typename = void>
    struct PushCValue {
        static void exec(lua_State *, const T &) {
            static_assert(sizeof(T) != sizeof(T),
                          "Type T is not a CValue");
        }
    };
    
    template <>
    struct PushCValue<std::string> {
        static void exec(lua_State *L, const std::string &val) {
            lua_pushlstring(L, val.c_str(), val.length());
        }
    };

    template <>
    struct PushCValue<bool> {
        static void exec(lua_State *L, bool val) {
            lua_pushboolean(L, val);
        }
    };

    template <>
    struct PushCValue<std::nullptr_t> {
        static void exec(lua_State *L, std::nullptr_t) {
            lua_pushnil(L);
        }
    };
    
    template <typename T>
    struct PushCValue<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
        static void exec(lua_State *L, T val) {
            lua_pushnumber(L, static_cast<lua_Number>(val));
        }
    };
            
    template <typename T>
    struct PushCValue<T, typename std::enable_if<std::is_integral<T>::value ||
                                                 std::is_enum<T>::value>::type> {
        static void exec(lua_State *L, T val) {
            lua_pushinteger(L, static_cast<lua_Integer>(val));
        }
    };

    template <typename T>
    struct PushCValue<T, typename std::enable_if<std::is_class<T>::value &&
                                                 !IsUserDataPtrType<T>::value>::type> {
        static void exec(lua_State *L, const T &val) {
            UserData<T>::pushPtr(L, std::unique_ptr<T>{new T(val)});
        }
    };
    
    template <typename T>
    struct PushCValue<T, typename std::enable_if<IsUserDataPtrType<T>::value>::type> {
        template <typename Tf>
        static void exec(lua_State *L, Tf &&val) {
            using Ti = typename IsUserDataPtrType<T>::inner_type;
            UserData<Ti>::pushPtr(L, std::forward<Tf>(val));
        }
    };
}

template <typename T>
void pushCValue(lua_State *L, T &&val) {
    detail::template PushCValue<typename std::decay<T>::type>
        ::exec(L, std::forward<T>(val));
}

inline void pushCValues(lua_State *L) {
}

template <typename T, typename... Ts>
void pushCValues(lua_State *L, T &&val, Ts &&... vals) {
    pushCValue(L, std::forward<T>(val));
    pushCValues(L, std::forward<Ts>(vals)...);
}

namespace detail {
    template <typename T, typename = void>
    struct ToCValue {
        static T exec(lua_State *, int) {
            static_assert(sizeof(T) != sizeof(T),
                          "Type T is not a CValue");
        }
    };
    
    template <>
    struct ToCValue<std::string> {
        static std::string exec(lua_State *L, int index) {
            luaL_checktype(L, index, LUA_TSTRING);
            size_t len;
            auto cptr = luaL_checklstring(L, index, &len);
            return { cptr, len };
        }
    };

    template <>
    struct ToCValue<bool> {
        static bool exec(lua_State *L, int index) {
            luaL_checktype(L, index, LUA_TBOOLEAN);
            return lua_toboolean(L, index) != 0;
        }
    };

    template <typename T>
    struct ToCValue<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
        static T exec(lua_State *L, int index) {
            luaL_checktype(L, index, LUA_TNUMBER);
            return static_cast<T>(lua_tonumber(L, index));
        }
    };

    template <typename T>
    struct ToCValue<T, typename std::enable_if<std::is_integral<T>::value ||
                                               std::is_enum<T>::value>::type> {
        static T exec(lua_State *L, int index) {
            luaL_checktype(L, index, LUA_TNUMBER);
            return static_cast<T>(lua_tointeger(L, index));
        }
    };

    template <typename T>
    struct ToCValue<T, typename std::enable_if<std::is_class<T>::value &&
                                               !IsUserDataPtrType<T>::value>::type> {
        static T &exec(lua_State *L, int index) {
            return UserData<T>::getRef(L, index);
        }
    };

    template <typename T>
    struct ToCValue<T &, typename std::enable_if<std::is_class<T>::value &&
                                                 !IsUserDataPtrType<T>::value>::type> {
        static T &exec(lua_State *L, int index) {
            return UserData<T>::getRef(L, index);
        }        
    };
  
    template <typename T>
    struct ToCValue<T, typename std::enable_if<IsUserDataPtrType<T>::value>::type> {
        static T exec(lua_State *L, int index) {
            using Ti = typename IsUserDataPtrType<T>::inner_type;
            return UserData<Ti>::template toPtr<T>(L, index);
        }
    };

    template <typename T>
    struct ToCValue<T &, typename std::enable_if<IsUserDataPtrType<T>::value>::type> {
        static T &exec(lua_State *L, int index) {
            using Ti = typename IsUserDataPtrType<T>::inner_type;
            return UserData<Ti>::template getPtr<T>(L, index);
        }
    };
}

template <typename T>
T toCValue(lua_State *L, int index) {
    return detail::template ToCValue<T>::exec(L, index);
}

namespace detail {
    template <typename... Ts, size_t... Seq>
    std::tuple<Ts...> toCValuesHelper(lua_State *L,
                                      int narg,
                                      NumericSequence<Seq...>) {
        return std::tuple<Ts...>{toCValue<Ts>(L, narg+Seq)...};
    }
}

template <typename... Ts>
std::tuple<Ts...> toCValues(lua_State *L, int narg) {
    return detail::toCValuesHelper<Ts...>(L, narg, AscendingSequence<sizeof...(Ts)>());
}

#endif
