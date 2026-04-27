#pragma once
#include <memory>

class IDateProvider {
   public:
    // C++23 standard way to disable copying/moving for an abstract base class
    IDateProvider()                                = default;
    IDateProvider(const IDateProvider&)            = delete;
    IDateProvider& operator=(const IDateProvider&) = delete;
    IDateProvider(IDateProvider&&)                 = delete;
    IDateProvider& operator=(IDateProvider&&)      = delete;
    virtual ~IDateProvider()                       = default;
    [[nodiscard]] virtual int getDayOfWeek() const = 0;
};

std::unique_ptr<IDateProvider> createDateProvider();
