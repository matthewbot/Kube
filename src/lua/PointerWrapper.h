#ifndef POINTERWRAPPER_H
#define POINTERWRAPPER_H

#include "util/Variant.h"

#include <lua.hpp>
#include <type_traits>
#include <memory>

template <typename T>
struct IsPointerWrapperType;

template <typename T>
class PointerWrapper {
public:
    using Inner = T;
    
    template <typename TPtr>
    static void push(lua_State *L, TPtr &&ptr);

    static PointerWrapper<T> *to(lua_State *L, int index);

    T *get();    
    
    template <typename TPtr>
    TPtr *getPtr();

    static const std::string &getMetatableName();
    
private:
    template <typename TPtr>
    PointerWrapper(TPtr &&ptr) : ptrvar(std::forward<TPtr>(ptr)) { }
    
    Variant<T *, std::shared_ptr<T>, std::unique_ptr<T>> ptrvar;
};

#include "PointerWrapper.icc"

#endif
