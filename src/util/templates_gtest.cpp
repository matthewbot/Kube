#include "templates.h"
#include <type_traits>
#include <memory>
#include <gtest/gtest.h>

static_assert(
    std::is_same<
        typename MapTypeSequence<std::add_volatile, int, float, double>::seq,
        TypeSequence<volatile int, volatile float, volatile double>
    >::value,
    "MapTypeSequence broken");

static_assert(
    TypeToIndex<float, int, float, double>::value == 1,
    "TypeToIndex broken");

static_assert(
    std::is_same<
        typename IndexToType<2, int, float, double>::type,
        double
    >::value,
    "IndexToType broken");

static_assert(
    std::is_same<
        FindType<std::is_const, int, float, double, const int, const float>::type,
        const int
    >::value,
    "FindType broken");

static_assert(
    std::is_same<
        typename AscendingSequence<4>::seq,
        NumericSequence<0, 1, 2, 3>
    >::value,
    "AscendingSequence broken");

static double testFn(int i, float f, double d) {
    return i + f + d;
}

TEST(Templates, InvokeWithTuple) {
    auto tup = std::make_tuple(1, 2.0f, 3.0);
    auto result = invokeWithTuple(testFn, tup);
    static_assert(std::is_same<decltype(result), double>::value,
                  "invokeWithTuple produced wrong result");
    EXPECT_DOUBLE_EQ(result, 6);
}

struct Foo {
    int i;
    Foo(int i) : i(i) { }
    
    int bar(int j) const { return i * j; }
    double bar(double j) const { return i * j; }
};

TEST(Templates, BindMemberFunction) {
    Foo f(6);
    auto imemfn = bindMemberFunction(f, static_cast<int (Foo::*)(int) const>(&Foo::bar));
    EXPECT_EQ(42, imemfn(7));
    auto dmemfn = bindMemberFunction(f, static_cast<double (Foo::*)(double) const>(&Foo::bar));
    EXPECT_EQ(3, dmemfn(.5));
}

TEST(Templates, CopyOrMove) {
    auto shared = std::make_shared<int>(42);
    auto shared_cpy = copyOrMove(shared);
    EXPECT_EQ(shared, shared_cpy);
    auto unique = std::unique_ptr<int>(new int{42});
    auto unique_mvd = copyOrMove(unique);
    EXPECT_FALSE(unique);
    EXPECT_EQ(42, *unique_mvd);
}
