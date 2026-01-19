#include <greeter/IDateProvider.h>

#include <chrono>

namespace dt = std::chrono;
class ChronoDateProvider : public IDateProvider {
   public:
    int getDayOfWeek() const override {
        const auto        now {dt::system_clock::now()};
        const dt::weekday week_day {dt::floor<dt::days>(now)};

        // C++ weekday is 0-6 for Sun-Sat
        return static_cast<int>(week_day.c_encoding());
    }
};

std::unique_ptr<IDateProvider> createDateProvider() {
    return std::make_unique<ChronoDateProvider>();
}
