#include <cstdint>
#include <iostream>
#include <print>

void inspectChar(char character) {
    // A char IS an 8-bit integer — cast it to see the number
    uint8_t raw = static_cast<uint8_t>(character);

    std::println("Char   : {}", character);                // A
    std::println("Decimal: {}", static_cast<int>(raw));    // 65
    std::println("Hex    : {:x}", static_cast<int>(raw));  // 41
    std::print("Bits   : ");
    constexpr int bitSize = 8;
    for (int index = bitSize - 1; index >= 0; --index) {
        std::print("{}", (raw >> index) & 1);  // 01000001
    }
    std::println();

    std::println("Address: {}", static_cast<void*>(&character));
    std::println("Size   : {} byte", sizeof(character));  // always 1
}

// UTF-8: multi-byte characters
void inspectUTF8() {
    // The Euro sign encodes as 3 bytes in UTF-8: E2 82 AC
    const char euro[] = "â¬";  // 3 bytes + NUL
    std::println("UTF-8 bytes for the Euro sign:");
    constexpr int euroBytes = 3;
    for (int index = 0; index < euroBytes; ++index) {
        std::println("  byte[{}] = 0x{:02X}", index, static_cast<uint8_t>(euro[index]));
    }
    // Output:
    //   byte[0] = 0xE2   (11100010)
    //   byte[1] = 0x82   (10000010)
    //   byte[2] = 0xAC   (10101100)
}

// Reading raw memory back as a char
void readFromMemory() {
    constexpr uint8_t rawByteValue = 0x41;  // store the number 65
    uint8_t           rawByte      = rawByteValue;
    char*             asChar       = reinterpret_cast<char*>(&rawByte);
    std::println("Raw 0x41 as char: {}", *asChar);  // prints 'A'
}

int main() {
    inspectChar('A');
    inspectUTF8();
    readFromMemory();
}
