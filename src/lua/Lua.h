#ifndef LUA_H
#define LUA_H

#include <string>
#include <stdexcept>

extern "C" {
    struct lua_State;
}

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

    void doFile(const std::string &filename);
    void call(const std::string &funcname);

    Lua &operator=(const Lua &) = delete;

    operator lua_State *() { return L; }
    
private:
    lua_State *L;
};

#endif
