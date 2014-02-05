#ifndef BINDINGS_H
#define BINDINGS_H

#include <lua.hpp>
#include <tuple>

template <typename T>
void pushCValue(lua_State *L, T &&val);

template <typename... Ts>
void pushCValues(lua_State *L, Ts &&... vals);

template <typename T>
T toCValue(lua_State *L, int index);

template <typename... Ts>
std::tuple<Ts...> toCValues(lua_State *L, int narg);

#include "bindings.icc"

#endif
