#ifndef USERDATA_H
#define USERDATA_H

#include "util/Variant.h"

#include <lua.hpp>
#include <type_traits>
#include <memory>

#include <iostream>
#include <cassert>

// TODO, make getters member functions
// Two statics, pushPointerWrapper and toPointerWrapper
template <typename T>
class UserData {
    static_assert(!std::is_pointer<T>::value && !std::is_reference<T>::value,
                  "Trying to instantiate UserData for pointer or reference type");
public:
    // Push a pointer type to the stack. 
    template <typename TPtr>
    static void pushPtr(lua_State *L, TPtr &&ptr);

    // Gets a reference to a pointer type on the stack.
    // Types must match exactly, and nil results in an error.
    template <typename TPtr>
    static TPtr &getPtr(lua_State *L, int narg);

    // Converts a stack entry to a pointer type.
    // Smart pointers are converted to raw pointers.
    // nil is converted to a nullptr of the requested type.
    template <typename TPtr>
    static TPtr toPtr(lua_State *L, int narg);

    // Converts a stack entry to a reference to the underlying type,
    // checking for nil and null.
    static T &getRef(lua_State *L, int narg);

    // Invoke destructor on a stack entry.
    // Should only be needed in __gc metamethod.
    static void destroy(lua_State *L, int narg);
    
    // TODO move to MetatableBuilder
    static std::string getMetatableName();
    
private:
    // TODO extract as toRawPtr, use in getRef
    // std::is_pointer<TPtr> === std::true_type
    template <typename TPtr>
    static TPtr toPtrHelper(lua_State *L, int narg, std::true_type);

    // std::is_pointer<TPtr> === std::false_type
    template <typename TPtr>
    static TPtr toPtrHelper(lua_State *L, int narg, std::false_type);

    static T *toRawPtr(lua_State *L, int narg);
    
    static UserData<T> &toUserData(lua_State *L, int narg);

    template <typename TPtr>
    UserData(TPtr &&ptr);
    
    // Userdata can have no ownership, shared ownership, or unique ownership
    Variant<T *, std::shared_ptr<T>, std::unique_ptr<T>> ptr;
};

#include "UserData.icc"

#endif
