#include "lua/bindings.h"
#include "lua/Lua.h"
#include "util/templates.h"
#include <gtest/gtest.h>

struct Foo {
    int i;

    Foo(int i) : i(i) { }

    bool operator==(const Foo &f) const { return i == f.i; }
};

class BindingsTest : public testing::Test {
protected:
    Lua lua;

    virtual void SetUp() {
        luaL_newmetatable(lua, PointerWrapper<Foo>::getMetatableName().c_str());
        luaL_newmetatable(lua, PointerWrapper<const Foo>::getMetatableName().c_str());
        lua_pop(lua, 2);
    }
};

static const std::string teststr = "Hello World";
static const int testint = 42;
static const float testflt = 1.234e5;
static const double testdbl = 4.321e-21;
static const bool testbool = true;

enum class TestEnum { E0, E1, E2, E3, E4 };
static const TestEnum testenum = TestEnum::E2;

TEST_F(BindingsTest, PushCValue) {
    pushCValue(lua, teststr);
    EXPECT_EQ(teststr, lua_tostring(lua, -1));

    pushCValue(lua, testint);
    EXPECT_EQ(testint, lua_tointeger(lua, -1));

    pushCValue(lua, testflt);
    EXPECT_DOUBLE_EQ(testflt, lua_tonumber(lua, -1));

    pushCValue(lua, testdbl);
    EXPECT_DOUBLE_EQ(testdbl, lua_tonumber(lua, -1));

    pushCValue(lua, testbool);
    EXPECT_TRUE(lua_toboolean(lua, -1));

    pushCValue(lua, testenum);
    EXPECT_EQ(static_cast<int>(testenum), lua_tointeger(lua, -1));
}

TEST_F(BindingsTest, PushCValues) {
    pushCValues(lua, teststr, testint, testflt, testdbl, testbool, testenum);
    EXPECT_EQ(teststr, lua_tostring(lua, 1));
    EXPECT_EQ(testint, lua_tointeger(lua, 2));
    EXPECT_DOUBLE_EQ(testflt, lua_tonumber(lua, 3));
    EXPECT_DOUBLE_EQ(testdbl, lua_tonumber(lua, 4));
    EXPECT_TRUE(lua_toboolean(lua, 5));
    EXPECT_EQ(static_cast<int>(testenum), lua_tointeger(lua, -1));
}

TEST_F(BindingsTest, ToCValue) {
    pushCValue(lua, teststr);
    EXPECT_EQ(teststr, toCValue<std::string>(lua, -1));

    pushCValue(lua, testint);
    EXPECT_EQ(testint, toCValue<int>(lua, -1));

    pushCValue(lua, testflt);
    EXPECT_DOUBLE_EQ(testflt, toCValue<float>(lua, -1));

    pushCValue(lua, testdbl);
    EXPECT_DOUBLE_EQ(testdbl, toCValue<double>(lua, -1));

    pushCValue(lua, testbool);
    EXPECT_TRUE(toCValue<bool>(lua, -1));

    pushCValue(lua, testenum);
    EXPECT_EQ(testenum, toCValue<TestEnum>(lua, -1));
}

TEST_F(BindingsTest, ToCValueException) {
    pushCValue(lua, 42);
    EXPECT_THROW(toCValue<std::string>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<Foo &>(lua, -1), ToCValueException);

    pushCValue(lua, std::string{"Hello"});
    EXPECT_THROW(toCValue<int>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<bool>(lua, -1), ToCValueException);

    lua_pushnil(lua);
    EXPECT_THROW(toCValue<float>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<Foo &>(lua, -1), ToCValueException);

    EXPECT_THROW(toCValue<int>(lua, 42), ToCValueException);
    EXPECT_THROW(toCValue<Foo &>(lua, 42), ToCValueException);
}

TEST_F(BindingsTest, ToCValues) {
    pushCValues(lua, teststr, testint, testflt, testdbl, testbool, testenum);
    auto vals = toCValues<std::string, int, float, double, bool, TestEnum>(lua, 1);
    EXPECT_EQ(teststr, std::get<0>(vals));
    EXPECT_EQ(testint, std::get<1>(vals));
    EXPECT_EQ(testflt, std::get<2>(vals));
    EXPECT_EQ(testdbl, std::get<3>(vals));
    EXPECT_EQ(testbool, std::get<4>(vals));
    EXPECT_EQ(testenum, std::get<5>(vals));
}

TEST_F(BindingsTest, ByValue) {
    Foo testfoo{42};
    pushCValue(lua, testfoo);
    EXPECT_EQ(testfoo, toCValue<Foo>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<Foo &>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<const Foo &>(lua, -1));
}

TEST_F(BindingsTest, PushAndToPtr) {
    Foo testfoo{42};

    pushCValue(lua, &testfoo);
    EXPECT_EQ(testfoo, toCValue<Foo>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<Foo &>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<const Foo &>(lua, -1));
    EXPECT_EQ(&testfoo, toCValue<Foo *>(lua, -1));
    EXPECT_EQ(&testfoo, toCValue<Foo *&>(lua, -1));
    EXPECT_THROW(toCValue<std::unique_ptr<Foo>>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<std::shared_ptr<Foo>>(lua, -1), ToCValueException);

    std::unique_ptr<Foo> testfoo_uptr{new Foo(testfoo)};
    pushCValue(lua, std::move(testfoo_uptr));
    EXPECT_FALSE(testfoo_uptr.get());
    EXPECT_EQ(testfoo, toCValue<Foo>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<Foo &>(lua, -1));
    EXPECT_EQ(testfoo, toCValue<const Foo &>(lua, -1));
    EXPECT_EQ(testfoo, *toCValue<Foo *>(lua, -1));
    EXPECT_EQ(testfoo, *toCValue<const Foo *>(lua, -1));
    EXPECT_EQ(testfoo, *toCValue<std::unique_ptr<Foo>>(lua, -1)); // should move unique_ptr
    EXPECT_EQ(nullptr, toCValue<std::unique_ptr<Foo>>(lua, -1)); // unique_ptr has been moved
    EXPECT_EQ(nullptr, toCValue<std::unique_ptr<Foo> &>(lua, -1));
    EXPECT_EQ(nullptr, toCValue<const std::unique_ptr<Foo> &>(lua, -1));
    EXPECT_THROW(toCValue<Foo *&>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<std::shared_ptr<Foo>>(lua, -1), ToCValueException);
    
    auto testfoo_sptr = std::make_shared<Foo>(55);
    pushCValue(lua, testfoo_sptr);
    EXPECT_TRUE(testfoo_sptr.get());
    EXPECT_EQ(*testfoo_sptr, toCValue<Foo>(lua, -1));
    EXPECT_EQ(*testfoo_sptr, toCValue<Foo &>(lua, -1));
    EXPECT_EQ(*testfoo_sptr, toCValue<const Foo &>(lua, -1));
    EXPECT_EQ(testfoo_sptr.get(), toCValue<Foo *>(lua, -1));
    EXPECT_EQ(testfoo_sptr.get(), toCValue<const Foo *>(lua, -1));
    EXPECT_EQ(testfoo_sptr, toCValue<std::shared_ptr<Foo>>(lua, -1));
    EXPECT_EQ(testfoo_sptr, toCValue<std::shared_ptr<Foo> &>(lua, -1));
    EXPECT_EQ(testfoo_sptr, toCValue<const std::shared_ptr<Foo> &>(lua, -1));
    EXPECT_THROW(toCValue<Foo *&>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<std::unique_ptr<Foo>>(lua, -1), ToCValueException);
}

TEST_F(BindingsTest, PushRef) {
    Foo testfoo{55};
    
    pushCValue(lua, std::ref(testfoo));
    EXPECT_EQ(testfoo, toCValue<Foo>(lua, -1));
    EXPECT_EQ(&testfoo, &toCValue<Foo &>(lua, -1));
    EXPECT_EQ(&testfoo, &toCValue<const Foo &>(lua, -1));
    EXPECT_THROW(toCValue<std::shared_ptr<Foo>>(lua, -1), ToCValueException);

    pushCValue(lua, std::cref(testfoo));
    EXPECT_EQ(testfoo, toCValue<Foo>(lua, -1));
    EXPECT_THROW(toCValue<Foo &>(lua, -1), ToCValueException);
    EXPECT_EQ(&testfoo, &toCValue<const Foo &>(lua, -1));
    EXPECT_THROW(toCValue<std::shared_ptr<Foo>>(lua, -1), ToCValueException);
}

TEST_F(BindingsTest, PushAndToNull) {
    pushCValue(lua, nullptr);
    EXPECT_TRUE(lua_isnil(lua, -1));
    pushCValue(lua, std::unique_ptr<Foo>());
    EXPECT_TRUE(lua_isnil(lua, -1));
    pushCValue(lua, std::shared_ptr<Foo>());
    EXPECT_TRUE(lua_isnil(lua, -1));

    EXPECT_EQ(nullptr, toCValue<Foo *>(lua, 1));
    EXPECT_EQ(nullptr, toCValue<const Foo *>(lua, 1));
    EXPECT_EQ(nullptr, toCValue<std::unique_ptr<Foo>>(lua, 1));
    EXPECT_EQ(nullptr, toCValue<std::shared_ptr<Foo>>(lua, 1));

    EXPECT_THROW(toCValue<Foo>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<std::shared_ptr<Foo> &>(lua, -1), ToCValueException);
    EXPECT_THROW(toCValue<std::unique_ptr<Foo> &>(lua, -1), ToCValueException);
}

int invokeInt(int i) { return 2 * i; }
int invokeString(const std::string &str) { return str.length(); }
int invokeStringFoo(const std::string &str, Foo f) { return f.i + str.length(); }

TEST_F(BindingsTest, InvokeWithLuaArgs) {
    pushCValue(lua, 21);
    EXPECT_EQ(42, invokeWithLuaArgs<int>(lua, 1, &invokeInt));

    pushCValue(lua, std::string{"abcd"});
    EXPECT_EQ(4, invokeWithLuaArgs<const std::string &>(lua, 2, &invokeString));

    pushCValue(lua, Foo{1});
    auto tmp = invokeWithLuaArgs<const std::string &, Foo>(lua, 2, &invokeStringFoo);
    EXPECT_EQ(5, tmp);
}

TEST_F(BindingsTest, PushTables) {
    pushCValue(lua, std::vector<int>{1, 2, 3, 4});
    EXPECT_EQ(LUA_TTABLE, lua_type(lua, -1));
    EXPECT_EQ(4, lua_objlen(lua, -1));

    pushCValue(lua, std::vector<std::string>{"a", "b", "c"});
    lua_pushinteger(lua, 3);
    lua_gettable(lua, -2);
    EXPECT_STREQ("c", lua_tostring(lua, -1));
}

TEST_F(BindingsTest, PushToTables) {
    std::vector<int> vals{8, 6, 7, 5, 3, 0, 9};
    pushCValue(lua, vals);
    EXPECT_EQ(vals, toCValue<std::vector<int>>(lua, -1));
}
