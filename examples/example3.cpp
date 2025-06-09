#include "myproject/ModuleA.h"
#include "myproject/ModuleB.h"
#include <iostream>

int main()
{
    myproject::StringProcessor sp;
    myproject::Calculator calc;

    std::string text = "   Spaces   ";
    std::cout << "Removed spaces: " << sp.removeSpaces(text) << std::endl;

    std::cout << "10 / 2 = " << calc.divide(10.0, 2.0) << std::endl;

    return 0;
}