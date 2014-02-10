#include "lua/Lua.h"
#include <gtest/gtest.h>

TEST(LuaTest, Globals) {
    Lua lua;
    lua.setGlobal("global", std::string{"Hello World"});
    EXPECT_EQ("Hello World", lua.getGlobal<std::string>("global"));
}

TEST(LuaTest, Call) {
    Lua lua;
    static const char *code =
        "local math = require \"math\"\n"
        "function foo(a, b)\n"
        "  return math.sqrt(a*a + b*b)\n"
        "end\n";
    lua.doString(code);

    auto result = lua.call<double, double>("foo", 3, 4);
    EXPECT_EQ(5, result);
}

TEST(LuaTest, CallVoid) {
    Lua lua;
    static const char *code =
        "function foo(a)\n"
        "    globalvar = a\n"
        "end\n";
    lua.doString(code);

    lua.call<void>("foo", std::string{"Hello World"});
    EXPECT_EQ("Hello World", lua.getGlobal<std::string>("globalvar"));
}
