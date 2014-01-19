#include "util/Optional.h"
#include <gtest/gtest.h>
#include <memory>

TEST(Optional, Instantiate) {
    Optional<int> a;
    Optional<char> b;
    Optional<std::unique_ptr<int>> c;
}

TEST(Optional, HoldsValue) {
    Optional<int> a = 5;
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_EQ(5, *a);
    a = 10;
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_EQ(10, *a);
}

TEST(Optional, None) {
    Optional<int> a;
    EXPECT_FALSE(static_cast<bool>(a));
    a = 42;
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_EQ(42, *a);
    a = None;
    EXPECT_FALSE(static_cast<bool>(a));
}

TEST(Optional, Destructors) {
    static int cons = 0;
    static int des = 0;
    struct A {
        A() { cons++; }
        A(const A &) { cons++; }
        ~A() { des++; }
    };

    Optional<A> a;
    ASSERT_EQ(0, cons);
    ASSERT_EQ(0, des);
    
    Optional<A> b{A{}};
    ASSERT_EQ(2, cons);
    ASSERT_EQ(1, des);

    b = None;
    ASSERT_EQ(2, cons);
    ASSERT_EQ(2, des);

    {
        Optional<A> c{A{}};
    }
    ASSERT_EQ(4, cons);
    ASSERT_EQ(4, des);
}

TEST(Optional, Copy) {
    Optional<int> o42 = 42;
    Optional<int> o7 = 7;
    Optional<int> n;
    
    Optional<int> a{o42};
    ASSERT_TRUE(static_cast<bool>(a));
    EXPECT_EQ(42, *a);

    a = o7;
    ASSERT_TRUE(static_cast<bool>(a));
    EXPECT_EQ(7, *a);

    a = n;
    EXPECT_FALSE(static_cast<bool>(a));
}

TEST(Optional, Move) {
    Optional<std::unique_ptr<int>> a{new int (42)};

    Optional<std::unique_ptr<int>> b = std::move(a);
    EXPECT_FALSE(static_cast<bool>(a));
    ASSERT_TRUE(static_cast<bool>(b));
    EXPECT_EQ(42, **b);
}
