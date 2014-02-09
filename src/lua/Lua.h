#ifndef LUA_H
#define LUA_H

#include <string>
#include <stdexcept>
#include <lua.hpp>

class LuaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Lua {
public:
    Lua();
    Lua(const Lua &) = delete;
    Lua(Lua &&lua);
    ~Lua();

    template <typename V>
    void setGlobal(const std::string &name, V &&val);
    template <typename V>
    V getGlobal(const std::string &name);

    void doFile(const std::string &filename);
    void doString(const std::string &code);

    template <typename Result, typename... Args>
    Result call(const std::string &name, Args&&... args);

    Lua &operator=(const Lua &) = delete;

    operator lua_State *() { return L; }
    
private:
    lua_State *L;

    void checkLoadResult(int result, const char *name);
    void pcallWrapper(int nargs, int nresults, const char *name);
};

#include "Lua.icc"

#endif
