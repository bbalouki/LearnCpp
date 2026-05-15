#include <cstdint>
#include <iostream>

using namespace std;

void inspectChar(char c) {
    // A char IS an 8-bit integer — cast it to see the number
    uint8_t raw = static_cast<uint8_t>(c);

    cout << "Char   : " << c << endl;                // A
    cout << "Decimal: " << (int)raw << endl;         // 65
    cout << "Hex    : " << hex << (int)raw << endl;  // 41
    cout << "Bits   : ";
    for (int i = 7; i >= 0; --i) cout << ((raw >> i) & 1);  // 01000001
    cout << endl;

    cout << "Address: " << (void*)&c << endl;
    cout << "Size   : " << sizeof(c) << " byte" << endl;  // always 1
}

// UTF-8: multi-byte characters
void inspectUTF8() {
    // The Euro sign encodes as 3 bytes in UTF-8: E2 82 AC
    const char euro[] = "â¬";  // 3 bytes + NUL
    printf("UTF-8 bytes for the Euro sign:\n");
    for (int i = 0; i < 3; ++i) printf("  byte[%d] = 0x%02X\n", i, (uint8_t)euro[i]);
    // Output:
    //   byte[0] = 0xE2   (11100010)
    //   byte[1] = 0x82   (10000010)
    //   byte[2] = 0xAC   (10101100)
}

// Reading raw memory back as a char
void readFromMemory() {
    uint8_t rawByte = 0x41;  // store the number 65
    char*   asChar  = reinterpret_cast<char*>(&rawByte);
    cout << "Raw 0x41 as char: " << *asChar << endl;  // prints 'A'
}

int main() {
    inspectChar('A');
    inspectUTF8();
    readFromMemory();
}
