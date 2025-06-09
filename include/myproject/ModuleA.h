#ifndef MYPROJECT_MODULEA_H_fi84aj
#define MYPROJECT_MODULEA_H_fi84aj

#include <string>

/**
 * @file ModuleA.h
 * @brief Module containing string processing utilities.
 * @ingroup myproject
 * @author GitHub Copilot
 * @date Generated on: @date
 *
 * @details This file contains the StringProcessor class which provides
 * various string manipulation utilities for the myproject namespace.
 */

namespace myproject
{

/**
 * @class StringProcessor
 * @brief Utility class for string manipulation operations.
 *
 * @details This class provides various methods to manipulate strings
 * including reversing, converting to uppercase, and removing spaces.
 * All methods are designed to be non-destructive, returning new string
 * instances rather than modifying the input.
 *
 * @note All methods are thread-safe as they don't modify internal state.
 *
 * @see std::string
 *
 * @code
 * myproject::StringProcessor processor;
 * std::string original = "Hello World";
 * std::string reversed = processor.reverse(original);  // "dlroW olleH"
 * std::string uppercase = processor.toUpper(original); // "HELLO WORLD"
 * std::string noSpaces = processor.removeSpaces(original); // "HelloWorld"
 * @endcode 
 */

class StringProcessor
{
public:
    /**
     * @brief Reverses the characters of a string
     *
     * This function takes a string as input and returns a new string with the
     * characters in reverse order. For example, "hello" becomes "olleh".
     *
     * @code
     * StringProcessor processor;
     *
     * // Basic reversal
     * std::string normal = "hello";
     * std::string reversed = processor.reverse(normal); // reversed = "olleh"
     *
     * // Palindrome check
     * std::string word = "radar";
     * bool isPalindrome = (word == processor.reverse(word)); // true
     *
     * // Multi-word phrases
     * std::string phrase = "Time flies";
     * std::string backwards = processor.reverse(phrase); // "seilf emiT"
     * @endcode
     * @param input The string to be reversed
     * @return A new string containing the characters of the input in reverse
     * order
     */
    std::string reverse(const std::string& input);

    /**
     * @brief Converts a string to uppercase
     *
     * Takes an input string and returns a new string with all characters
     * converted to their uppercase equivalents. The original string
     * remains unchanged.
     *
     * @param input The string to be converted to uppercase
     * @return A new string with all characters in uppercase
     *
     * @note This function only affects ASCII characters; non-ASCII characters
     * may not be properly converted depending on the locale
     */
    std::string toUpper(const std::string& input);

    /**
     * @brief Removes all whitespace characters from a string.
     *
     * This function takes a string and returns a new string with all spaces
     * removed. For example, "hello world" becomes "helloworld".
     *
     * @param input The string from which to remove spaces
     * @return A new string with all spaces removed
     */
    std::string removeSpaces(const std::string& input);
};

} // namespace myproject

#endif // MYPROJECT_MODULEA_H_fi84aj