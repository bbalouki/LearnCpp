#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <print>

// Dissect any float into its IEEE 754 fields
void dissectFloat(float value) {
    uint32_t         bits;
    constexpr size_t floatSize = 4;
    std::memcpy(&bits, &value, floatSize);  // safe type-pun — no undefined behavior

    constexpr int      signShift     = 31;
    constexpr int      exponentShift = 23;
    constexpr uint32_t exponentMask  = 0xFF;
    constexpr uint32_t mantissaMask  = 0x7FFFFF;
    constexpr int      bias          = 127;

    uint32_t sign     = bits >> signShift;
    uint32_t exponent = (bits >> exponentShift) & exponentMask;
    uint32_t mantissa = bits & mantissaMask;

    std::println("\nfloat = {}", value);
    std::println("Raw hex : 0x{:08X}", bits);
    std::println("Sign    : {}         ({})", sign, sign ? "negative" : "positive");
    std::println(
        "Exponent: {}         (biased), actual = {}", exponent, static_cast<int>(exponent) - bias
    );
    std::println("Mantissa: 0x{:06X}", mantissa);

    // Reconstruct value from raw fields
    double check = std::pow(-1.0, sign) * std::pow(2.0, static_cast<int>(exponent) - bias) *
                   (1.0 + static_cast<double>(mantissa) / (1 << exponentShift));
    std::println("Verify  : {:.10f}", check);
}

// Why 0.1 + 0.2 != 0.3
void floatPrecision() {
    float valA = 0.1f, valB = 0.2f;
    std::println("\n0.1f + 0.2f = {:.20f}", valA + valB);  // NOT exactly 0.3
    std::println("0.3f        = {:.20f}", 0.3f);

    // Correct way: compare with epsilon
    constexpr float epsilonValue = 1e-6f;
    bool            isEqual      = std::fabs((valA + valB) - 0.3f) < epsilonValue;
    std::println("Equal within 1e-6: {}", isEqual ? "true" : "false");
}

// double: 64 bits (1+11+52)
void dissectDouble(double doubleVal) {
    uint64_t         bits;
    constexpr size_t doubleSize = 8;
    std::memcpy(&bits, &doubleVal, doubleSize);

    constexpr int      signShift     = 63;
    constexpr int      exponentShift = 52;
    constexpr uint64_t exponentMask  = 0x7FF;
    constexpr uint64_t mantissaMask  = 0x000FFFFFFFFFFFFFull;
    constexpr int      bias          = 1023;

    uint64_t sign     = bits >> signShift;
    uint64_t exponent = (bits >> exponentShift) & exponentMask;
    uint64_t mantissa = bits & mantissaMask;
    std::println(
        "\ndouble {:.4f}: sign={} exp={} (actual {}) mantissa=0x{:013X}",
        doubleVal,
        sign,
        exponent,
        static_cast<long long>(exponent) - bias,
        mantissa
    );
}

// Special IEEE 754 bit patterns
void specials() {
    float            inf  = 1.0f / 0.0f;
    float            nan  = 0.0f / 0.0f;
    float            ninf = -1.0f / 0.0f;
    uint32_t         bitsInf, bitsNaN, bitsNinf;
    constexpr size_t floatSize = 4;
    std::memcpy(&bitsInf, &inf, floatSize);
    std::memcpy(&bitsNaN, &nan, floatSize);
    std::memcpy(&bitsNinf, &ninf, floatSize);
    std::println(
        "\n+Inf = 0x{:08X}   -Inf = 0x{:08X}   NaN = 0x{:08X}", bitsInf, bitsNinf, bitsNaN
    );
    // +Inf = 0x7F800000  (exp=0xFF, mantissa=0)
    // NaN  = 0x7FC00000  (exp=0xFF, mantissa != 0)
}

int main() {
    dissectFloat(6.5f);
    dissectFloat(0.1f);
    floatPrecision();
    dissectDouble(3.14159265358979);
    specials();
}
