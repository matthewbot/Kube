#ifndef LUA_H
#define LUA_H

#include <string>

extern "C" {
    struct lua_State;
}

class Lua {
public:
    Lua();
    ~Lua();

    void runFile(const std::string &filename);
    
private:
    lua_State *L;
};

#endif
