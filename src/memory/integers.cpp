#include <cstdint>
#include <cstring>
#include <iostream>

using namespace std;

// Print every bit of any integer
template <typename T>
void printBits(T value, const char* label) {
    cout << label << " (" << sizeof(T) * 8 << "-bit): ";
    for (int i = sizeof(T) * 8 - 1; i >= 0; --i) {
        cout << ((value >> i) & 1);
        if (i % 8 == 0 && i != 0)
            cout << '_';  // group bytes visually
    }
    cout << "  dec=" << (long long)value << "  hex=0x" << hex << (unsigned long long)value << dec
         << endl;
}

// Two's complement: signed vs unsigned same bits
void twosComplement() {
    int8_t  s = -1;
    uint8_t u;
    memcpy(&u, &s, 1);  // copy raw bytes — no value conversion
    printf("int8_t  -1 stored as: 0x%02X = %u unsigned\n", u, u);  // 0xFF = 255

    int8_t  s2 = -56;
    uint8_t u2;
    memcpy(&u2, &s2, 1);
    printf("int8_t -56 stored as: 0x%02X = %u unsigned\n", u2, u2);  // 0xC8 = 200
}

// Endianness: how a 32-bit int lays out in RAM
void endianness() {
    uint32_t value = 0x12345678;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);

    printf("\n0x12345678 in memory (byte-by-byte):\n");
    for (int i = 0; i < 4; ++i) printf("  Address +%d: 0x%02X\n", i, bytes[i]);
    // On little-endian (x86): 0x78  0x56  0x34  0x12
    // On big-endian (network): 0x12  0x34  0x56  0x78
}

// Reconstruct an int from raw bytes
void buildFromBytes() {
    uint8_t  raw[4] = {0x78, 0x56, 0x34, 0x12};  // little-endian layout
    uint32_t result;
    memcpy(&result, raw, 4);
    printf("\nReconstructed: %u (0x%X)\n", result, result);  // 305419896
}

// Overflow behavior
void overflow() {
    uint8_t u = 255;
    u++;                                  // wraps: 255 + 1 = 0  (modulo 2^8)
    printf("\nuint8_t 255+1 = %u\n", u);  // 0

    int8_t s = 127;
    s++;  // UNDEFINED BEHAVIOR for signed overflow
    printf("int8_t  127+1 = %d (UB — likely -128 on x86)\n", s);
}

int main() {
    printBits((uint8_t)200, "uint8_t  200");
    printBits((int8_t)-56, " int8_t  -56");
    printBits((uint32_t)305419896, "uint32_t 0x12345678");
    printBits((int32_t)-1, " int32_t   -1");
    twosComplement();
    endianness();
    buildFromBytes();
    overflow();
}
