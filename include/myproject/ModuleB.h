#ifndef MYPROJECT_MODULEB_H_3j4hjm
#define MYPROJECT_MODULEB_H_3j4hjm

/**
 * @file ModuleB.h
 * @brief Contains the Calculator class for basic arithmetic operations
 * @ingroup myproject
 *
 * This file defines a Calculator class that provides fundamental arithmetic
 * operations like addition, subtraction, multiplication, and division.
 */

namespace myproject
{

/**
 * @class Calculator
 * @brief A simple calculator for basic arithmetic operations
 *
 * The Calculator class provides methods for performing fundamental
 * arithmetic operations on double-precision floating-point numbers.
 * It handles the four basic operations: addition, subtraction,
 * multiplication, and division.
 *
 * @code
 * myproject::Calculator calc;
 *
 * // Basic operations
 * double sum = calc.add(5.0, 3.0);       // Returns 8.0
 * double diff = calc.subtract(10.0, 4.0); // Returns 6.0
 * double product = calc.multiply(2.5, 3.0); // Returns 7.5
 * double quotient = calc.divide(9.0, 3.0);  // Returns 3.0
 *
 * // Chaining operations
 * double result = calc.multiply(calc.add(2.0, 3.0), calc.subtract(10.0, 5.0));
 * // (2+3)*(10-5) = 25.0
 * @endcode
 */
class Calculator
{
public:
    /**
     * @brief Adds two double values together.
     *
     * This function takes two double precision floating-point numbers
     * and returns their sum.
     *
     * @param a The first double value to add
     * @param b The second double value to add
     * @return The sum of a and b as a double
     */
    double add(double a, double b);

    /**
     * @brief Subtracts one number from another.
     *
     * This function takes two double values and returns their difference.
     *
     * @param a The number to subtract from (the minuend).
     * @param b The number to subtract (the subtrahend).
     * @return The result of subtracting b from a (a - b).
     */
    double subtract(double a, double b);

    /**
     * @brief Multiplies two numbers together.
     *
     * This function takes two double precision floating point numbers
     * and returns their product.
     *
     * @param a The first number to multiply.
     * @param b The second number to multiply.
     * @return The product of a and b.
     */
    double multiply(double a, double b);

    /**
     * @brief Divides two numbers.
     *
     * This function takes two double values and returns their quotient.
     * Be careful when passing zero as the second parameter to avoid division by
     * zero errors.
     *
     * @param a The numerator.
     * @param b The denominator (should not be zero).
     * @return The result of dividing a by b.
     * @throws std::invalid_argument If b is zero.
     */
    double divide(double a, double b);
};

} // namespace myproject

#endif // MYPROJECT_MODULEB_H_3j4hjm