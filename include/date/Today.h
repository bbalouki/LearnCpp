#pragma once

#include <chrono>
#include <format>
#include <string>

namespace Date {
inline std::string Today() { return std::format("{:%A}", std::chrono::system_clock::now()); }
}  // namespace Date
