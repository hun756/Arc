#include <gtest/gtest.h>
#include "myproject/ModuleB.h"

using namespace myproject;

TEST(CalculatorTest, Addition) {
    Calculator calc;
    EXPECT_DOUBLE_EQ(calc.add(2.0, 3.0), 5.0);
    EXPECT_DOUBLE_EQ(calc.add(-2.0, 3.0), 1.0);
    EXPECT_DOUBLE_EQ(calc.add(0.0, 0.0), 0.0);
}

TEST(CalculatorTest, Subtraction) {
    Calculator calc;
    EXPECT_DOUBLE_EQ(calc.subtract(5.0, 3.0), 2.0);
    EXPECT_DOUBLE_EQ(calc.subtract(2.0, 3.0), -1.0);
    EXPECT_DOUBLE_EQ(calc.subtract(0.0, 0.0), 0.0);
}

TEST(CalculatorTest, Multiplication) {
    Calculator calc;
    EXPECT_DOUBLE_EQ(calc.multiply(2.0, 3.0), 6.0);
    EXPECT_DOUBLE_EQ(calc.multiply(-2.0, 3.0), -6.0);
    EXPECT_DOUBLE_EQ(calc.multiply(0.0, 5.0), 0.0);
}

TEST(CalculatorTest, Division) {
    Calculator calc;
    EXPECT_DOUBLE_EQ(calc.divide(6.0, 2.0), 3.0);
    EXPECT_DOUBLE_EQ(calc.divide(5.0, -2.0), -2.5);
    EXPECT_THROW(calc.divide(5.0, 0.0), std::invalid_argument);
}