#include "myproject/ModuleA.h"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>

namespace myproject
{

std::string StringProcessor::reverse(const std::string& input)
{
    std::string result = input;
    std::reverse(result.begin(), result.end());
    return result;
}

std::string StringProcessor::toUpper(const std::string& input)
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringProcessor::removeSpaces(const std::string& input)
{
    std::string result;
    std::copy_if(input.begin(), input.end(), std::back_inserter(result),
                 [](char c) { return !std::isspace(c); });
    return result;
}

} // namespace myproject