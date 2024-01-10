#include "arc.hpp"
#include <gtest/gtest.h>

// A dummy function to test
int add(int a, int b) {
    return a + b;
}

// Test case for the add function
TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(10, 20), 30);
}

// Main function
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
