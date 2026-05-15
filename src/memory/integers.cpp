#include <cstdint>
#include <cstring>
#include <iostream>
#include <print>

// Print every bit of any integer
template <typename T>
void printBits(T value, const char* label) {
    constexpr int bitSize = sizeof(T) * 8;
    std::print("{} ({}-bit): ", label, bitSize);
    for (int index = bitSize - 1; index >= 0; --index) {
        std::print("{}", (value >> index) & 1);
        constexpr int byteGroup = 8;
        if (index % byteGroup == 0 && index != 0)
            std::print("_");  // group bytes visually
    }
    std::println(
        "  dec={}  hex=0x{:x}",
        static_cast<long long>(value),
        static_cast<unsigned long long>(value)
    );
}

// Two's complement: signed vs unsigned same bits
void twosComplement() {
    int8_t           signedVal = -1;
    uint8_t          unsignedVal;
    constexpr size_t byteSize = 1;
    std::memcpy(&unsignedVal, &signedVal, byteSize);  // copy raw bytes — no value conversion
    std::println(
        "int8_t  -1 stored as: 0x{:02X} = {} unsigned", unsignedVal, unsignedVal
    );  // 0xFF = 255

    int8_t  signedVal2 = -56;
    uint8_t unsignedVal2;
    std::memcpy(&unsignedVal2, &signedVal2, byteSize);
    std::println(
        "int8_t -56 stored as: 0x{:02X} = {} unsigned", unsignedVal2, unsignedVal2
    );  // 0xC8 = 200
}

// Endianness: how a 32-bit int lays out in RAM
void endianness() {
    uint32_t value = 0x12345678;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);

    std::println("\n0x12345678 in memory (byte-by-byte):");
    constexpr int byteCount = 4;
    for (int index = 0; index < byteCount; ++index) {
        std::println("  Address +{}: 0x{:02X}", index, bytes[index]);
    }
    // On little-endian (x86): 0x78  0x56  0x34  0x12
    // On big-endian (network): 0x12  0x34  0x56  0x78
}

// Reconstruct an int from raw bytes
void buildFromBytes() {
    uint8_t          raw[4] = {0x78, 0x56, 0x34, 0x12};  // little-endian layout
    uint32_t         result;
    constexpr size_t byteCount = 4;
    std::memcpy(&result, raw, byteCount);
    std::println("\nReconstructed: {} (0x{:X})", result, result);  // 305419896
}

// Overflow behavior
void overflow() {
    constexpr uint8_t maxUint8    = 255;
    uint8_t           unsignedVal = maxUint8;
    unsignedVal++;                                      // wraps: 255 + 1 = 0  (modulo 2^8)
    std::println("\nuint8_t 255+1 = {}", unsignedVal);  // 0

    constexpr int8_t maxInt8   = 127;
    int8_t           signedVal = maxInt8;
    signedVal++;  // UNDEFINED BEHAVIOR for signed overflow
    std::println("int8_t  127+1 = {} (UB — likely -128 on x86)", signedVal);
}

int main() {
    printBits(static_cast<uint8_t>(200), "uint8_t  200");
    printBits(static_cast<int8_t>(-56), " int8_t  -56");
    printBits(static_cast<uint32_t>(305419896), "uint32_t 0x12345678");
    printBits(static_cast<int32_t>(-1), " int32_t   -1");
    twosComplement();
    endianness();
    buildFromBytes();
    overflow();
}
