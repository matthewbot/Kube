#include <gtest/gtest.h>
#include "lua/MetatableBuilder.h"
#include "lua/Lua.h"

class TestClass {
public:
    TestClass(int val) : val(val), nested(nullptr) { }

    void setVal(int val) { this->val = val; }
    
    int getVal() { return val; }
    int getVal() const { return val; }

    TestClass &getSelf() { return *this; }

    bool operator==(const TestClass &other) const { return val == other.val; }
    
    int val;
    TestClass *nested;
};

TEST(MetatableBuilder, Construct) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .constructor<int>("new");

    static const char *testcode =
        "return TestClass:new(42)\n";
    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(42, toCValue<const TestClass &>(lua, 1).getVal());
}

TEST(MetatableBuilder, VoidMethod) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .function("setVal", &TestClass::setVal);

    TestClass testobj{2};
    pushCValue(lua, std::ref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "testobj:setVal(42)";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 0, 0));
    EXPECT_EQ(42, testobj.val);
}

TEST(MetatableBuilder, Method) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .function("getVal", &TestClass::getVal);

    TestClass testobj{2};
    pushCValue(lua, std::ref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "return testobj:getVal() * 3";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(6, toCValue<int>(lua, 1));
}

TEST(MetatableBuilder, RefMethod) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .function_ref("getSelf", &TestClass::getSelf);

    TestClass testobj{2};
    pushCValue(lua, std::ref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "return testobj:getSelf()";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(&testobj, toCValue<TestClass *>(lua, 1));
}

TEST(MetatableBuilder, ConstMethod) {
    Lua lua;
    MetatableBuilder<const TestClass>(lua, "ConstTestClass")
        .function("getVal", &TestClass::getVal);

    TestClass testobj{2};
    pushCValue(lua, std::cref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "return testobj:getVal() * 3";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(6, toCValue<int>(lua, 1));
}

TEST(MetatableBuilder, Getter) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .getter("val", &TestClass::val);

    TestClass testobj{2};
    pushCValue(lua, testobj);
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "return testobj.val * 3";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(6, toCValue<int>(lua, 1));
}

TEST(MetatableBuilder, ConstGetter) {
    Lua lua;
    MetatableBuilder<const TestClass>(lua, "ConstTestClass")
        .getter("val", &TestClass::val);

    TestClass testobj{2};
    pushCValue(lua, std::cref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "return testobj.val * 3";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(6, toCValue<int>(lua, 1));
}

TEST(MetatableBuilder, Setter) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .setter("val", &TestClass::val);

    TestClass testobj{2};
    pushCValue(lua, std::ref(testobj));
    lua_setglobal(lua, "testobj");
    
    static const char *testcode =
        "testobj.val = 42";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 0, 0));
    EXPECT_EQ(42, testobj.val);
}

TEST(MetatableBuilder, NestedGetterSetters) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .field("val", &TestClass::val)
        .field("nested", &TestClass::nested);

    TestClass testobj{2};
    TestClass nestedobj{42};
    testobj.nested = &nestedobj;
    nestedobj.nested = &nestedobj;

    pushCValue(lua, std::ref(testobj));
    lua_setglobal(lua, "testobj");

    static const char *testcode =
        "assert(testobj.nested.nested == testobj.nested)\n"
        "testobj.nested.nested = nil\n"
        "assert(testobj.nested.nested == nil)\n"
        "return testobj.nested.val\n";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 1, 0));
    EXPECT_EQ(42, toCValue<int>(lua, 1));
    EXPECT_EQ(nullptr, nestedobj.nested);
}

TEST(MetatableBuilder, Index) {
    using StringArray = std::array<std::string, 3>;
    Lua lua;
    MetatableBuilder<StringArray>(lua, "StringArray")
        .index<StringArray::size_type, std::string>();

    StringArray array;
    array[2] = "Woop";
    pushCValue(lua, std::ref(array));
    lua_setglobal(lua, "stringarray");
    
    static const char *testcode =
        "assert(stringarray[2] == \"Woop\")\n"
        "stringarray[0] = \"Hello World\"\n";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 0, 0));
    EXPECT_EQ("Hello World", array[0]);
}

TEST(MetatableBuilder, OpEquals) {
    Lua lua;
    MetatableBuilder<TestClass>(lua, "TestClass")
        .op_equals();

    pushCValue(lua, TestClass{4});
    lua_setglobal(lua, "a");
    pushCValue(lua, TestClass{4});
    lua_setglobal(lua, "b");

    static const char *testcode =
        "assert(a == b)\n";

    ASSERT_EQ(0, luaL_loadstring(lua, testcode));
    ASSERT_EQ(0, lua_pcall(lua, 0, 0, 0));
}
