/**
 * @file Greeter.h
 * @brief Declares the public API for the Greeter library.
 *
 * This is the primary header you should include when using
 * the library:
 * @code
 * #include <greeter/Greeter.h>
 * @endcode
 */
#pragma once

#include <memory>
#include <string>

#include "IDateProvider.h"

/**
 * @brief Provides a friendly greeting.
 *
 * Generates a greeting string that can be displayed to the user.
 *
 * @return A std::string containing the greeting.
 *
 * @code
 * #include <iostream>
 * #include <greeter/Greeter.h>
 *
 * int main() {
 *   std::cout << get_greeting(); // Prints the greeting
 * }
 * @endcode
 */
std::string get_greeting();

/**
 * @brief Provides the os name .
 *
 * This function generates a greeting string that can be
 * displayed to the user. It's the core feature of the
 * Greeter library.
 *
 * @return A std::string containing the greeting.
 *
 * @code
 * #include <iostream>
 * #include <greeter/Greeter.h>
 * int main() {
 *   std::cout << get_os_string(); // Prints the greeting
 * }
 * @endcode
 */
std::string get_os_string();
std::string get_internal_greeting();

class Greeter {
   public:
    // Default constructor uses ChronoDateProvider
    Greeter();

    // Greeter allows for a different dependency
    // to be injected for testing/flexibility
    explicit Greeter(std::unique_ptr<IDateProvider> provider);
    explicit Greeter(std::string name);
    void        setEnthusiasm(float level);
    float       getEnthusiasm() const;
    std::string greet() const;
    std::string greet_name() const;
    std::string simple_greet() const;

   private:
    std::string                    name_ {"User"};
    float                          enthusiasm_ {0.5F};
    std::unique_ptr<IDateProvider> date_provider_;
};
