// src/main.cpp
#include "myproject/ModuleA.h"
#include "myproject/ModuleB.h"
#include <iostream>

int main()
{
    myproject::StringProcessor sp;
    myproject::Calculator calc;

    // StringProcessor test
    std::string test = "Hello World";
    std::cout << "Original: " << test << std::endl;
    std::cout << "Reversed: " << sp.reverse(test) << std::endl;
    std::cout << "Uppercase: " << sp.toUpper(test) << std::endl;
    std::cout << "No spaces: " << sp.removeSpaces(test) << std::endl;

    // Calculator test
    double a = 10.0, b = 5.0;
    std::cout << "\nCalculator Results:" << std::endl;
    std::cout << a << " + " << b << " = " << calc.add(a, b) << std::endl;
    std::cout << a << " - " << b << " = " << calc.subtract(a, b) << std::endl;
    std::cout << a << " * " << b << " = " << calc.multiply(a, b) << std::endl;
    std::cout << a << " / " << b << " = " << calc.divide(a, b) << std::endl;

    return 0;
}
