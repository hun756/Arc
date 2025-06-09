#include "myproject/ModuleB.h"
#include <iostream>

int main()
{
    myproject::Calculator calc;

    double a = 15.0, b = 5.0;
    std::cout << "Calculator Example\n";
    std::cout << a << " + " << b << " = " << calc.add(a, b) << std::endl;
    std::cout << a << " * " << b << " = " << calc.multiply(a, b) << std::endl;

    return 0;
}