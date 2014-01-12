#ifndef BINDINGS_H
#define BINDINGS_H

#include "lua/UserData.h"
#include "util/templates.h"

#include <lua.hpp>
#include <typeinfo>
#include <type_traits>
#include <memory>

template <typename T>
using IsBuiltinLuaType =
    std::integral_constant<bool,
                           std::is_same<T, std::string>::value ||
                           std::is_same<T, bool>::value ||
                           std::is_enum<T>::value ||
                           std::is_arithmetic<T>::value>;

template <typename T, typename = void>
struct RemoveReferenceFromBuiltinLuaType {
    using type = T;
};

template <typename T>
struct RemoveReferenceFromBuiltinLuaType<
    T &, typename std::enable_if<IsBuiltinLuaType<T>::value>::type> {
    using type = T;
};

template <typename T, typename = void>
struct ValueBinding {
    static_assert(!IsBuiltinLuaType<T>::value, "Should not happen");
    
    // Pushing a value allocates a copy and gives lua ownership
    static void push(lua_State *L, T val) {
        UserData<T>::pushPtr(L, std::unique_ptr<T>(
                                 new T{std::move(val)}));
    }

    // Converting to a value actually gives a const reference to it
    static T to(lua_State *L, int narg) {
        return UserData<T>::getRef(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for pointers to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Bindings for pointers to pointers not implemented");
    
    // Pushing a pointer makes a non-owning userdata
    static void push(lua_State *L, T *val) {
        UserData<T>::pushPtr(L, val);
    }

    // Converting to a pointer takes the address from the userdata
    static T *to(lua_State *L, int narg) {
        return UserData<T>::toPtr(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *&> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for pointers to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Bindings for pointers to pointers not implemented");

    // Pushing a reference to a pointer makes a non-owning userdata
    static void push(lua_State *L, T *val) {
        UserData<T>::pushPtr(L, val);
    }

    // Converting to a reference to a pointer gives a reference to the userdata's unowned pointer.
    static T *&to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<T *>(L, narg);
    }
};

template <typename T>
struct ValueBinding<T *const &> : public ValueBinding<T *> { };

template <typename T>
struct ValueBinding<T &> {
    static_assert(!IsBuiltinLuaType<T>::value, "Bindings for references to a builtin lua type not implemented");
    static_assert(!IsUserDataPtrType<T>::value, "Should not happen");
    
    // Pushing a reference makes a non-owning userdata
    static void push(lua_State *L, T &val) {
        UserData<T>::pushPtr(L, &val);
    }

    // Converting to a reference gives the contents of the userdata
    static T &to(lua_State *L, int narg) {
        return UserData<T>::getRef(L, narg);
    }
};

// Reference wrapper is treated like an actual reference
template <typename T>
struct ValueBinding<std::reference_wrapper<T>> : public ValueBinding<T &> { };

template <typename T>
struct ValueBinding<std::shared_ptr<T>> {
    // Pushing a shared_ptr makes a shared owner userdata
    static void push(lua_State *L, std::shared_ptr<T> val) {
        UserData<T>::pushPtr(L, std::move(val));
    }

    // Converting to a shared_ptr requires a shared owner userdata
    static std::shared_ptr<T> to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::shared_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<std::shared_ptr<T> &> {
    // A reference to a std::unique_ptr cannot be pushed.
    // Only pushing by value is supported.
    
    // Converting to a shared_ptr reference requires a shared owner userdata
    static std::shared_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::shared_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<const std::shared_ptr<T>> : public ValueBinding<std::shared_ptr<T>> { };

template <typename T>
struct ValueBinding<std::unique_ptr<T>> {
    // pushing a unique_ptr moves it into an owning Userdata
    static void push(lua_State *L, std::unique_ptr<T> &&val) {
        UserData<T>::pushPtr(L, std::move(val));
    }

    // Converting to a unique_ptr transfers ownership out of an owning Userdata
    static std::unique_ptr<T> to(lua_State *L, int narg) {
        return std::move(UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg));
    }
};

template <typename T>
struct ValueBinding<std::unique_ptr<T> &> {
    // A reference to a std::unique_ptr cannot be pushed.
    // Explicitly call get() and pass as a raw pointer instead.
    // This has different semantics, so we don't do it by default.

    // Converting to a unique_ptr reference gives access to the
    // UserData's unique_ptr but does not move.
    static std::unique_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg);
    }
};

template <typename T>
struct ValueBinding<const std::unique_ptr<T>> : public ValueBinding<std::unique_ptr<T>> { };

template <typename T>
struct ValueBinding<const std::unique_ptr<T> &> {
    // Converting to a const unique_ptr reference gives access to the
    // UserData's unique_ptr but does not move.
    static const std::unique_ptr<T> &to(lua_State *L, int narg) {
        return UserData<T>::template getPtr<std::unique_ptr<T>>(L, narg);
    }
};

// const values which are built in lua types are treated though they
// were non-const. For example, pushing an explicit const int pushes
// an int.
template <typename T>
struct ValueBinding<const T, typename std::enable_if<IsBuiltinLuaType<T>::value>::type> :
    public ValueBinding<T> { };

template <typename T>
struct ValueBinding<T, typename std::enable_if<std::is_integral<T>::value ||
                                               std::is_enum<T>::value>::type> {
    // Pushing an integer creates a lua integer
    static void push(lua_State *L, T val) {
        lua_pushinteger(L, static_cast<lua_Integer>(val));
    }

    // Converting to an integer casts from a lua integer
    static T to(lua_State *L, int narg) {
        return static_cast<T>(luaL_checkint(L, narg));
    }
};

template <typename T>
struct ValueBinding<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    // Pushing a floating point number creates a lua number
    static void push(lua_State *L, T val) {
        lua_pushnumber(L, static_cast<lua_Number>(val));
    }

    // Converting to a floating point number casts from a lua number
    static T to(lua_State *L, int narg) {
        return static_cast<T>(luaL_checknumber(L, narg));
    }
};

template <>
struct ValueBinding<std::string> {
    static void push(lua_State *L, const std::string &str) {
        lua_pushlstring(L, str.c_str(), str.length());
    }

    static std::string to(lua_State *L, int narg) {
        return {luaL_checkstring(L, narg)};
    }
};

template <>
struct ValueBinding<bool> {
    static void push(lua_State *L, bool b) {
        lua_pushboolean(L, b);
    }

    static bool to(lua_State *L, int narg) {
        return lua_toboolean(L, narg) != 0;
    }
};

template <typename B, typename T>
void pushCType(lua_State *L, T && val) {
    ValueBinding<B>::push(L, std::forward<T>(val));
}

inline void pushAllCType(lua_State *L, TypeSequence<>) {
}    

template <typename B, typename... Bs, typename T, typename... Ts>
void pushAllCType(lua_State *L, TypeSequence<B, Bs...>, T&& val, Ts&&... vals) {
    pushCType<B>(L, std::forward<T>(val));
    pushAllCType(L, TypeSequence<Bs...>(), std::forward<Ts>(vals)...);
}

template <typename T>
T toCType(lua_State *L, int narg) {
    return ValueBinding<T>::to(L, narg);
}

namespace detail {
    template <typename... Ts, size_t... Seq>
    std::tuple<Ts...> toAllHelper(lua_State* L,
                                  int narg,
                                  NumericSequence<Seq...>) {
        return std::tuple<Ts...>(toCType<Ts>(L, narg+Seq)...);
    }
}

template <typename... Ts>
std::tuple<Ts...> toAllCType(lua_State *L, int narg) {
    return detail::toAllHelper<Ts...>(L, narg, AscendingSequence<sizeof...(Ts)>());
}

#endif
