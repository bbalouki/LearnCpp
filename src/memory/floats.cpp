#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>

using namespace std;

// Dissect any float into its IEEE 754 fields
void dissectFloat(float f) {
    uint32_t bits;
    memcpy(&bits, &f, 4);  // safe type-pun — no undefined behavior

    uint32_t sign     = bits >> 31;
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t mantissa = bits & 0x7FFFFF;

    printf("\nfloat = %f\n", f);
    printf("Raw hex : 0x%08X\n", bits);
    printf("Sign    : %u         (%s)\n", sign, sign ? "negative" : "positive");
    printf("Exponent: %u         (biased), actual = %d\n", exponent, (int)exponent - 127);
    printf("Mantissa: 0x%06X\n", mantissa);

    // Reconstruct value from raw fields
    double check =
        pow(-1.0, sign) * pow(2.0, (int)exponent - 127) * (1.0 + (double)mantissa / (1 << 23));
    printf("Verify  : %.10f\n", check);
}

// Why 0.1 + 0.2 != 0.3
void floatPrecision() {
    float a = 0.1f, b = 0.2f;
    printf("\n0.1f + 0.2f = %.20f\n", a + b);  // NOT exactly 0.3
    printf("0.3f        = %.20f\n", 0.3f);

    // Correct way: compare with epsilon
    float eps   = 1e-6f;
    bool  equal = fabs((a + b) - 0.3f) < eps;
    printf("Equal within 1e-6: %s\n", equal ? "true" : "false");
}

// double: 64 bits (1+11+52)
void dissectDouble(double d) {
    uint64_t bits;
    memcpy(&bits, &d, 8);
    uint64_t sign     = bits >> 63;
    uint64_t exponent = (bits >> 52) & 0x7FF;
    uint64_t mantissa = bits & 0x000FFFFFFFFFFFFFull;
    printf(
        "\ndouble %.4f: sign=%llu exp=%llu (actual %lld) mantissa=0x%013llX\n",
        d,
        sign,
        exponent,
        (long long)exponent - 1023,
        mantissa
    );
}

// Special IEEE 754 bit patterns
void specials() {
    float    inf  = 1.0f / 0.0f;
    float    nan  = 0.0f / 0.0f;
    float    ninf = -1.0f / 0.0f;
    uint32_t b1, b2, b3;
    memcpy(&b1, &inf, 4);
    memcpy(&b2, &nan, 4);
    memcpy(&b3, &ninf, 4);
    printf("\n+Inf = 0x%08X   -Inf = 0x%08X   NaN = 0x%08X\n", b1, b3, b2);
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
