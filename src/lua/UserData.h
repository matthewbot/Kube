#ifndef USERDATA_H
#define USERDATA_H

#include "util/Variant.h"

#include <lua.hpp>
#include <type_traits>
#include <memory>

#include <iostream>
#include <cassert>

template <typename T>
struct IsUserDataPtrType : public std::false_type { };

template <typename T>
struct IsUserDataPtrType<const T> : public IsUserDataPtrType<T> { };

template <typename T>
struct IsUserDataPtrType<T *> : public std::true_type {
    using inner_type = T;
};

template <typename T>
struct IsUserDataPtrType<std::shared_ptr<T>> : public std::true_type {
    using inner_type = T;
};

template <typename T>
struct IsUserDataPtrType<std::unique_ptr<T>> : public std::true_type {
    using inner_type = T;
};

template <typename T>
struct UserData {
    static_assert(!std::is_pointer<T>::value && !std::is_reference<T>::value,
                  "Trying to instantiate UserData for pointer or reference type");
    
    // Userdata can have no ownership, shared ownership, or unique ownership
    Variant<T *, std::shared_ptr<T>, std::unique_ptr<T>> ptr;

    // Push a pointer type to the stack. 
    template <typename TPtr>
    static void pushPtr(lua_State *L, TPtr &&ptr) {
        if (!ptr) {
            lua_pushnil(L);
            return;
        }

        void *mem = lua_newuserdata(L, sizeof(UserData<T>));
        new (mem) UserData{{std::forward<TPtr>(ptr)}};
        std::string name = getMetatableName();
        luaL_getmetatable(L, name.c_str());
        lua_setmetatable(L, -2);
    }

    // Gets a reference to a pointer type on the stack.
    // Types must match exactly, and nil results in an error.
    template <typename TPtr>
    static TPtr &getPtr(lua_State *L, int narg) {
        static_assert(IsUserDataPtrType<TPtr>::value,
                      "Trying to convert userdata to an unsupported pointer type");
        
        auto &ud = toUserData(L, narg);
        auto tptrptr = ud.ptr.template getPtr<typename std::remove_cv<TPtr>::type>();
        if (!tptrptr) {
            throw std::runtime_error("Converting UserData to wrong pointer type");
        }
        return *tptrptr;
    }

    // Converts a stack entry to a pointer type.
    // Smart pointers are converted to raw pointers.
    // nil is converted to a nullptr of the requested type.

    template <typename TPtr>
    static TPtr toPtr(lua_State *L, int narg) {
        if (lua_isnil(L, narg)) {
            return nullptr;
        }

        // static if std::is_pointer<TPtr>
        return toPtrHelper<TPtr>(L, narg, typename std::is_pointer<TPtr>::type());
    }

private:
    // std::is_pointer<TPtr> === std::true_type
    template <typename TPtr>
    static TPtr toPtrHelper(lua_State *L, int narg, std::true_type) {
        auto &ud = toUserData(L, narg);

        // All possible ownership types can become a raw pointer
        return ud.ptr.template match<T *>(
            [](T *ptr) { return ptr; },
            [](const std::shared_ptr<T> &ptr) { return ptr.get(); },
            [](const std::unique_ptr<T> &ptr) { return ptr.get(); });
    }

    // std::is_pointer<TPtr> === std::false_type
    template <typename TPtr>
    static TPtr toPtrHelper(lua_State *L, int narg, std::false_type) {
        // smart pointer types must match exactly, so just use getPtr.
        return copyOrMove(getPtr<TPtr>(L, narg));
    }

public:
    // Converts a stack entry to a reference to the underlying type,
    // checking for nil and null.
    static T &getRef(lua_State *L, int narg) {
        if (std::is_const<T>::value && !checkMetatable(L, narg)) {
            return UserData<typename std::remove_const<T>::type>::getRef(L, narg);
        }
        
        auto ptr = toPtr<T *>(L, narg);
        if (ptr) {
            return *ptr;
        } else {
            throw std::runtime_error("Trying to convert nil/null userdata to reference");
        }
    }

    // Invoke destructor on a stack entry.
    // Should only be needed in __gc metamethod.
    static void destroy(lua_State *L, int narg) {
        auto &ud = toUserData(L, narg);
        ud.~UserData<T>();
    }

private:
    static UserData<T> &toUserData(lua_State *L, int narg) {
        std::string name = getMetatableName();
        return *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, name.c_str()));
    }

public:
    // TODO move to MetatableBuilder
    static std::string getMetatableName() {
        if (std::is_const<T>::value) {
            return std::string{"UD_C_"} + typeid(T).name();
        } else {
            return std::string{"UD_"} + typeid(T).name();
        }
    }

    static bool checkMetatable(lua_State *L, int narg) {
        int top = lua_gettop(L);
        
        luaL_getmetatable(L, getMetatableName().c_str());
        bool match = lua_getmetatable(L, narg) && lua_rawequal(L, -1, -2);
        lua_settop(L, top);

        return match;
    }
};

#endif
