#include "myproject/ModuleA.h"
#include <iostream>

int main()
{
    myproject::StringProcessor sp;
    std::string text = "Hello Example 1";

    std::cout << "Original: " << text << std::endl;
    std::cout << "Reversed: " << sp.reverse(text) << std::endl;

    return 0;
}