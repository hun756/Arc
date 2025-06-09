#include <gtest/gtest.h>
#include "myproject/ModuleA.h"

using namespace myproject;

TEST(StringProcessorTest, ReverseString) {
    StringProcessor sp;
    EXPECT_EQ(sp.reverse("hello"), "olleh");
    EXPECT_EQ(sp.reverse(""), "");
    EXPECT_EQ(sp.reverse("a"), "a");
}

TEST(StringProcessorTest, ToUpperCase) {
    StringProcessor sp;
    EXPECT_EQ(sp.toUpper("hello"), "HELLO");
    EXPECT_EQ(sp.toUpper("Hello World"), "HELLO WORLD");
    EXPECT_EQ(sp.toUpper("123"), "123");
}

TEST(StringProcessorTest, RemoveSpaces) {
    StringProcessor sp;
    EXPECT_EQ(sp.removeSpaces("hello world"), "helloworld");
    EXPECT_EQ(sp.removeSpaces("   spaces   "), "spaces");
    EXPECT_EQ(sp.removeSpaces("nospaces"), "nospaces");
}