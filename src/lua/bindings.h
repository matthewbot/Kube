#ifndef BINDINGS_H
#define BINDINGS_H

#include <lua.hpp>
#include <tuple>
#include <typeindex>

class LuaStackFrame {
public:
    LuaStackFrame(lua_State *&L) : L(L), start(lua_gettop(L)) { }
    ~LuaStackFrame() { lua_settop(L, start); }

    operator int() const { return start; }
    
private:
    lua_State *&L;
    int start;
};

template <typename T>
void pushCValue(lua_State *L, T &&val);

template <typename... Ts>
void pushCValues(lua_State *L, Ts &&... vals);

class ToCValueException : public std::exception {
public:
    ToCValueException(int argn, std::type_index expected, std::string msg=std::string{});

    int getArgn() const { return argn; }
    const std::type_index &getExpected() const { return expected; }
    const std::string &getMessage() const { return msg; }
    
    virtual const char *what() const throw();

private:
    int argn;
    std::type_index expected;
    std::string msg;
};

template <typename T>
T toCValue(lua_State *L, int index);

template <typename... Ts>
std::tuple<Ts...> toCValues(lua_State *L, int narg);

template <typename T, typename U>
void castCValue(lua_State *L, int index);

template <typename... Args, typename Callable>
auto invokeWithLuaArgs(lua_State *L, int narg, Callable &&call) ->
    typename std::result_of<Callable(Args...)>::type;

#include "bindings.icc"

#endif
