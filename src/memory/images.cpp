#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Pixel struct: exactly 3 bytes, no compiler padding
#pragma pack(push, 1)
struct RGB {
    uint8_t r, g, b;
};
struct RGBA {
    uint8_t r, g, b, a;
};
#pragma pack(pop)

// Raw image: a flat array of pixels in row-major order
struct Image {
    uint32_t    width, height;
    vector<RGB> pixels;  // pixel at (x,y) = pixels[y * width + x]

    Image(uint32_t w, uint32_t h) : width(w), height(h), pixels(w * h) {}

    RGB&       at(uint32_t x, uint32_t y) { return pixels[y * width + x]; }
    const RGB& at(uint32_t x, uint32_t y) const { return pixels[y * width + x]; }

    const uint8_t* rawBytes() const { return reinterpret_cast<const uint8_t*>(pixels.data()); }
    size_t         byteSize() const { return width * height * sizeof(RGB); }  // 3 per pixel
};

// Show how pixel (x,y) maps to a byte offset
void showLayout(const Image& img, uint32_t x, uint32_t y) {
    size_t     offset = (y * img.width + x) * sizeof(RGB);
    const RGB& px     = img.at(x, y);
    printf("Pixel (%u,%u) -> byte offset %zu from image start\n", x, y, offset);
    printf("  R=0x%02X  G=0x%02X  B=0x%02X\n", px.r, px.g, px.b);
    printf("  &R=%p  &G=%p  &B=%p\n", (void*)&px.r, (void*)&px.g, (void*)&px.b);
    // Note: &G == &R + 1  and  &B == &R + 2  (packed, no gaps)
}

// Grayscale: collapse 3 bytes per pixel to 1
void toGrayscale(const Image& src, vector<uint8_t>& out) {
    out.resize(src.width * src.height);
    for (size_t i = 0; i < src.pixels.size(); ++i) {
        const RGB& p = src.pixels[i];
        // ITU-R BT.601 perceptual luma formula
        out[i] = (uint8_t)(0.299f * p.r + 0.587f * p.g + 0.114f * p.b);
    }
}

// Write uncompressed BMP to disk
void writeBMP(const Image& img, const char* path) {
#pragma pack(push, 1)
    struct BMPHeader {
        char     sig[2] = {'B', 'M'};
        uint32_t fileSize;
        uint16_t reserved1 = 0, reserved2 = 0;
        uint32_t dataOffset = 54;
        uint32_t infoSize   = 40;
        int32_t  w, h;
        uint16_t planes = 1, bpp = 24;
        uint32_t comp = 0, imgSize = 0;
        int32_t  xppm = 2835, yppm = 2835;
        uint32_t clrs = 0, iclrs = 0;
    };
#pragma pack(pop)

    // BMP rows must be 4-byte aligned
    uint32_t  stride = (img.width * 3 + 3) & ~3;
    BMPHeader hdr;
    hdr.w        = img.width;
    hdr.h        = -(int32_t)img.height;  // negative = top-down row order
    hdr.fileSize = 54 + stride * img.height;

    ofstream f(path, ios::binary);
    f.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));

    vector<uint8_t> row(stride, 0);
    for (uint32_t y = 0; y < img.height; ++y) {
        for (uint32_t x = 0; x < img.width; ++x) {
            const RGB& px  = img.at(x, y);
            row[x * 3 + 0] = px.b;  // BMP stores in BGR order, not RGB!
            row[x * 3 + 1] = px.g;
            row[x * 3 + 2] = px.r;
        }
        f.write(reinterpret_cast<char*>(row.data()), stride);
    }
}

int main() {
    Image img(4, 2);
    img.at(0, 0) = {255, 0, 0};
    img.at(1, 0) = {0, 255, 0};
    img.at(2, 0) = {0, 0, 255};
    img.at(3, 0) = {255, 255, 0};
    img.at(0, 1) = {255, 0, 255};
    img.at(1, 1) = {0, 255, 255};
    img.at(2, 1) = {128, 128, 128};
    img.at(3, 1) = {255, 255, 255};

    printf("Total raw bytes: %zu\n", img.byteSize());  // 4*2*3 = 24

    showLayout(img, 2, 1);

    vector<uint8_t> gray;
    toGrayscale(img, gray);
    printf("Luma values: ");
    for (uint8_t v : gray) printf("%3u ", v);
    printf("\n");

    writeBMP(img, "output.bmp");
    printf("Wrote output.bmp (open in any image viewer)\n");
}
