#include "greeter/Greeter.h"

#include <stdexcept>

#include "InternalDetails.h"

std::string get_arch_string() {
#if defined(ARCH_64BIT)
    return "Compiled for 64-bit!";
#elif defined(ARCH_32BIT)
    return "Compiled for 32-bit!";
#else
    return "Unknow architecture.";
#endif
}

std::string get_internal_greeting() {
    return "Hello from " + InternalDetails::Name + " " + get_arch_string();
}

std::string get_greeting() { return "Hello from the modular greeter library"; }

void Greeter::setEnthusiasm(float level) {
    if (level < 0.0f || level > 1.0f) {
        throw std::out_of_range("Enthusiasm must be between 0.0 and 1.0");
    }
    enthusiasm_ = level;
}

float Greeter::getEnthusiasm() const { return enthusiasm_; }

// Forward declaration of the factory function
std::unique_ptr<IDateProvider> createDateProvider();

// Default constructor uses ChronoDateProvider
Greeter::Greeter() : date_provider_(createDateProvider()) {}

// Constructor that takes a custom provider
Greeter::Greeter(std::unique_ptr<IDateProvider> provider)
    : date_provider_(std::move(provider)) {}

Greeter::Greeter(std::string name) : name_(std::move(name)) {}

std::string Greeter::greet_name() const {
    return "Hello, " + name_ + "!";
}

std::string Greeter::simple_greet() const {
    std::string greeting{"Hello from the Greeter class"};
    int exclamations{static_cast<int>(enthusiasm_ * 3)};
    for (int i = 0; i < exclamations; ++i) {
        greeting += "!";
    }
    return greeting;
}

std::string Greeter::greet() const {
    int day{date_provider_->getDayOfWeek()};
    std::string day_str;
    switch (day) {
        case 1:
            day_str = "Happy Monday!";
            break;
        case 5:
            day_str = "Happy Friday!";
            break;
        default:
            day_str = "Have a nice day!";
            break;
    }
    return day_str + " Hello from the Greeter class!";
}
