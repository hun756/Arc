#include "arc.hpp"
#include <gtest/gtest.h>

TEST(AddTest, PositiveNumbers) {
    using  Arc::Arc;
    Arc sh1;
    EXPECT_EQ(sh1.testFunc(), 999);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
