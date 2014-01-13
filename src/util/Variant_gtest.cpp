#include "Variant.h"
#include <gtest/gtest.h>
#include <memory>

TEST(Variant, Instantiate) {
    struct NonCopyable {
        NonCopyable(const NonCopyable &) = delete;
        NonCopyable &operator=(const NonCopyable &) = delete;
    };
    
    Variant<int> a;
    Variant<int, char> b;
    Variant<int, char, std::unique_ptr<char>> c;
    Variant<std::unique_ptr<int>, NonCopyable> d;
}

TEST(Variant, Use) {
    Variant<int, unsigned int, std::string> a(42);
    EXPECT_EQ(0, a.getTag());
    ASSERT_NE(nullptr, a.getPtr<int>());
    EXPECT_EQ(42, *a.getPtr<int>());
    EXPECT_EQ(nullptr, a.getPtr<std::string>());

    const std::string msg = "Hello World";
    a = msg;
    EXPECT_EQ(2, a.getTag());
    EXPECT_EQ(nullptr, a.getPtr<int>());
    ASSERT_NE(nullptr, a.getPtr<std::string>());
    EXPECT_EQ(msg, *a.getPtr<std::string>());
}

TEST(Variant, ConstructDestruct) {
    static int acons = 0;
    static int ades = 0;
    static int bcons = 0;
    static int bdes = 0;

    struct A {
        A() { acons++; }
        A(const A &) { acons++; }
        ~A() { ades++; }
    };

    struct B {
        B() { bcons++; }
        B(const B &) { bcons++; }
        ~B() { bdes++; }
    };

    {
        Variant<A, B> v;
        ASSERT_EQ(2, acons);
        ASSERT_EQ(1, ades);
        ASSERT_EQ(0, bcons);
        ASSERT_EQ(0, bdes);

        v = B{};
        ASSERT_EQ(2, acons);
        ASSERT_EQ(2, ades);
        ASSERT_EQ(2, bcons);
        ASSERT_EQ(1, bdes);

        v = A{};
        ASSERT_EQ(4, acons);
        ASSERT_EQ(3, ades);
        ASSERT_EQ(2, bcons);
        ASSERT_EQ(2, bdes);
    }

    ASSERT_EQ(4, acons);
    ASSERT_EQ(4, ades);
    ASSERT_EQ(2, bcons);
    ASSERT_EQ(2, bdes);
}

TEST(Variant, Matching) {
    Variant<int, std::string, double> v;
    v = 5;

    v.match([](int i) { ASSERT_EQ(i, 5); },
            [](const std::string &str) { FAIL(); },
            [](double d) { FAIL(); });

    static const std::string msg = "Hello World";
    v = msg;

    v.match([](int i) { FAIL(); },
            [](const std::string &str) { ASSERT_EQ(msg, str); },
            [](double d) { FAIL(); });

    v = 3.14;
    
    v.match([](int i) { FAIL(); },
            [](const std::string &str) { FAIL(); },
            [](double d) { ASSERT_EQ(3.14, d); });
}
