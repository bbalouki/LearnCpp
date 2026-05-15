#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

// Pixel struct: exactly 3 bytes, no compiler padding
#pragma pack(push, 1)
struct RGB {
    uint8_t red, green, blue;
};
struct RGBA {
    uint8_t red, green, blue, alpha;
};
#pragma pack(pop)

// Raw image: a flat array of pixels in row-major order
struct Image {
    uint32_t         width, height;
    std::vector<RGB> pixels;  // pixel at (x,y) = pixels[y * width + x]

    Image(uint32_t widthVal, uint32_t heightVal)
        : width(widthVal), height(heightVal), pixels(widthVal * heightVal) {}

    RGB&       at(uint32_t posX, uint32_t posY) { return pixels[posY * width + posX]; }
    const RGB& at(uint32_t posX, uint32_t posY) const { return pixels[posY * width + posX]; }

    const uint8_t* rawBytes() const { return reinterpret_cast<const uint8_t*>(pixels.data()); }
    size_t         byteSize() const { return width * height * sizeof(RGB); }  // 3 per pixel
};

// Show how pixel (x,y) maps to a byte offset
void showLayout(const Image& image, uint32_t posX, uint32_t posY) {
    size_t     offset = (posY * image.width + posX) * sizeof(RGB);
    const RGB& pixel  = image.at(posX, posY);
    std::println("Pixel ({},{}) -> byte offset {} from image start", posX, posY, offset);
    std::println("  R=0x{:02X}  G=0x{:02X}  B=0x{:02X}", pixel.red, pixel.green, pixel.blue);
    std::println(
        "  &R={}  &G={}  &B={}",
        static_cast<const void*>(&pixel.red),
        static_cast<const void*>(&pixel.green),
        static_cast<const void*>(&pixel.blue)
    );
    // Note: &G == &R + 1  and  &B == &R + 2  (packed, no gaps)
}

// Grayscale: collapse 3 bytes per pixel to 1
void toGrayscale(const Image& source, std::vector<uint8_t>& output) {
    output.resize(source.width * source.height);
    for (size_t index = 0; index < source.pixels.size(); ++index) {
        const RGB& pixel = source.pixels[index];
        // ITU-R BT.601 perceptual luma formula
        constexpr float redWeight   = 0.299f;
        constexpr float greenWeight = 0.587f;
        constexpr float blueWeight  = 0.114f;
        output[index]               = static_cast<uint8_t>(
            redWeight * pixel.red + greenWeight * pixel.green + blueWeight * pixel.blue
        );
    }
}

// Write uncompressed BMP to disk
void writeBMP(const Image& image, const char* filePath) {
#pragma pack(push, 1)
    struct BMPHeader {
        char     signature[2] = {'B', 'M'};
        uint32_t fileSize;
        uint16_t reserved1 = 0, reserved2 = 0;
        uint32_t dataOffset = 54;
        uint32_t infoSize   = 40;
        int32_t  width, height;
        uint16_t planes = 1, bitsPerPixel = 24;
        uint32_t compression = 0, imageSize = 0;
        int32_t  xPixelsPerMeter = 2835, yPixelsPerMeter = 2835;
        uint32_t colorsUsed = 0, importantColors = 0;
    };
#pragma pack(pop)

    // BMP rows must be 4-byte aligned
    constexpr uint32_t alignmentMask = 3;
    uint32_t           stride        = (image.width * 3 + alignmentMask) & ~alignmentMask;
    BMPHeader          header;
    header.width  = static_cast<int32_t>(image.width);
    header.height = -static_cast<int32_t>(image.height);  // negative = top-down row order
    constexpr uint32_t headerSize = 54;
    header.fileSize               = headerSize + stride * image.height;

    std::ofstream file(filePath, std::ios::binary);
    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    std::vector<uint8_t> row(stride, 0);
    for (uint32_t posY = 0; posY < image.height; ++posY) {
        for (uint32_t posX = 0; posX < image.width; ++posX) {
            const RGB&    pixel         = image.at(posX, posY);
            constexpr int blueOffset    = 0;
            constexpr int greenOffset   = 1;
            constexpr int redOffset     = 2;
            row[posX * 3 + blueOffset]  = pixel.blue;  // BMP stores in BGR order, not RGB!
            row[posX * 3 + greenOffset] = pixel.green;
            row[posX * 3 + redOffset]   = pixel.red;
        }
        file.write(reinterpret_cast<char*>(row.data()), stride);
    }
}

int main() {
    Image image(4, 2);
    image.at(0, 0) = {255, 0, 0};
    image.at(1, 0) = {0, 255, 0};
    image.at(2, 0) = {0, 0, 255};
    image.at(3, 0) = {255, 255, 0};
    image.at(0, 1) = {255, 0, 255};
    image.at(1, 1) = {0, 255, 255};
    image.at(2, 1) = {128, 128, 128};
    image.at(3, 1) = {255, 255, 255};

    std::println("Total raw bytes: {}", image.byteSize());  // 4*2*3 = 24

    showLayout(image, 2, 1);

    std::vector<uint8_t> gray;
    toGrayscale(image, gray);
    std::print("Luma values: ");
    for (uint8_t value : gray) std::print("{:3} ", value);
    std::println();

    writeBMP(image, "output.bmp");
    std::println("Wrote output.bmp (open in any image viewer)");
}
