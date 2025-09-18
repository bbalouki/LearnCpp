#pragma once
#include <memory>

// Abstract interface for a date provider
class IDateProvider {
   public:
    virtual ~IDateProvider() = default;
    // 0=Sun, 1=Mon, ...
    virtual int getDayOfWeek() const = 0;
};

// Factory function to create the real provider
std::unique_ptr<IDateProvider> createDateProvider();
