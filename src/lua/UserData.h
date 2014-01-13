#ifndef USERDATA_H
#define USERDATA_H

#include "util/Variant.h"

#include <lua.hpp>
#include <type_traits>
#include <memory>

template <typename T>
struct IsUserDataPtrType : public std::false_type { };

template <typename T>
struct IsUserDataPtrType<T *> : public std::true_type { };

template <typename T>
struct IsUserDataPtrType<std::shared_ptr<T>> : public std::true_type { };

template <typename T>
struct IsUserDataPtrType<std::unique_ptr<T>> : public std::true_type { };

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
        luaL_getmetatable(L, typeid(T).name());
        lua_setmetatable(L, -2);
    }

    // Convert a stack entry back to a pointer type.
    template <typename TPtr>
    static TPtr &getPtr(lua_State *L, int narg) {
        static_assert(IsUserDataPtrType<TPtr>::value,
                      "Trying to convert userdata to an unsupported pointer type");

        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        auto tptrptr = ud.ptr.template getPtr<TPtr>();
        if (!tptrptr) {
            throw std::runtime_error("Converting UserData to wrong pointer type");
        }
        return *tptrptr;
    }

    // Converts a stack entry to a raw pointer,
    // also checking for nils and giving nullptr
    static T *toPtr(lua_State *L, int narg) {
        if (lua_isnil(L, narg)) {
            return nullptr;
        }

        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        return ud.ptr.template match<T *>(
            [](T *ptr) { return ptr; },
            [](const std::shared_ptr<T> &ptr) { return ptr.get(); },
            [](const std::unique_ptr<T> &ptr) { return ptr.get(); });
    }

    // Converts a stack entry to a reference,
    // checking for null
    static T &getRef(lua_State *L, int narg) {
        auto ptr = toPtr(L, narg);
        if (ptr) {
            return *ptr;
        } else {
            throw std::runtime_error("Trying to convert null userdata to reference");
        }
    }

    // Invoke destructor on a stack entry.
    // Should only be needed in __gc metamethod.
    static void destroy(lua_State *L, int narg) {
        auto &ud = *static_cast<UserData<T> *>(
            luaL_checkudata(L, narg, typeid(T).name()));
        ud.~UserData<T>();
    }
};

#endif
