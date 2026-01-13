#pragma once

#include <chrono>
#include <concepts>
#include <iostream>
#include <limits>
#include <random>

namespace Utils {
long getCppStandard() {
    // Visual Studio is non-conforming in support for __cplusplus
    // (unless you set a specific compiler flag, which you probably haven't)
    // In Visual Studio 2015 or newer we can use _MSVC_LANG instead
    // [See](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/)
#if defined(_MSVC_LANG)
    return _MSVC_LANG;
#elif defined(_MSC_VER)
    // If we're using an older version of Visual Studio, bail out
    return -1;
#else
    // __cplusplus is the intended way to query the language standard code (as
    // defined by the language standards)
    return __cplusplus;
#endif
}

// This program prints the C++ language standard your compiler is currently
// using Freely redistributable, courtesy of
// [learncpp.com](https://www.learncpp.com/cpp-tutorial/what-language-standard-is-my-compiler-using/)
void printCppStandard() {
    long             standard     = getCppStandard();
    static const int numStandards = 7;
    // The C++26 stdCode is a placeholder since the exact code won't be
    // determined until the standard is finalized
    static const long  stdCode[numStandards] = {199711L, 201103L, 201402L, 201703L,
                                                202002L, 202302L, 202612L};
    static const char* stdName[numStandards] = {"Pre-C++11", "C++11", "C++14", "C++17",
                                                "C++20",     "C++23", "C++26"};

    if (standard == -1) {
        std::cout << "Error: Unable to determine your language standard.  Sorry.\n";
        return;
    }

    for (int i = 0; i < numStandards; ++i) {
        // If the reported version is one of the finalized standard codes
        // then we know exactly what version the compiler is running
        if (standard == stdCode[i]) {
            std::cout << "Your compiler is using " << stdName[i] << " (language standard code "
                      << standard << "L)\n";
            break;
        }

        // If the reported version is between two finalized standard codes,
        // this must be a preview / experimental support for the next upcoming
        // version.
        if (standard < stdCode[i]) {
            std::cout << "Your compiler is using a preview/pre-release of " << stdName[i]
                      << " (language standard code " << standard << "L)\n";
            break;
        }
    }
}

void handleBadInputs() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

}  // namespace Utils

namespace Random {
static std::random_device rd_device;
using Clock = std::chrono::steady_clock;
using Seed  = std::seed_seq;

// Seed source (can be slow, so call once)
// Create a seed sequence from the random device
auto        seed_source = Clock::now().time_since_epoch().count();
static Seed seed{static_cast<Seed::result_type>(seed_source),
                 rd_device(),
                 rd_device(),
                 rd_device(),
                 rd_device(),
                 rd_device(),
                 rd_device(),
                 rd_device(),
                 rd_device()};

// Mersenne Twister engine, seeded once
static std::mt19937 mt{seed};

template <typename T>
    requires std::integral<T> || std::floating_point<T>
T randn(T min, T max) {
    // It's generally better to make the random engine static or thread_local
    // to avoid re-seeding and re-creating it on every call, which can be slow
    // and might lead to less "random" sequences if called very frequently.

    if constexpr (std::integral<T>) {
        // For integral types, use uniform_int_distribution
        std::uniform_int_distribution<T> dist(min, max);
        // Generate a random integer in the range [min, max]
        return dist(mt);
    } else if constexpr (std::floating_point<T>) {
        // For floating-point types, use uniform_real_distribution
        std::uniform_real_distribution<T> dist(min, max);
        // Generate a random real number in the range [min, max) or [min, max]
        // (behavior for max depends on library implementation but usually
        // inclusive for real)
        return dist(mt);
}
}

// This function is a convenience overload for integral types
// It allows calling randn with two different types for min and max
// where R is the return type, S is the type of min, and T is the type of max.
template <typename R, typename S, typename T>
R randn(S min, T max) {
    return randn<R>(static_cast<R>(min), static_cast<R>(max));
}

}  // namespace Random

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202302L) || (__cplusplus >= 202302L)
// Define a concept that checks if a type is a scoped enum
    template <typename T>
    concept IsScopedEnum = std::is_scoped_enum_v<T>;
#else
    template <typename T>
    concept IsScopedEnum = std::is_enum_v<T> && !std::is_convertible_v<T, int>;
#endif

// This function allows us to convert the enumerators of scoped enum
// classes to integral values using unary operator+.
template <IsScopedEnum EnumClass>
constexpr auto operator+(EnumClass a) noexcept {
    return static_cast<std::underlying_type_t<EnumClass>>(a);
}

class Timer {
   private:
    using Clock  = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<Clock> m_beg{Clock::now()};

   public:
    void reset() { m_beg = Clock::now(); }

    double elapsed() const {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};
