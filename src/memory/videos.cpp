#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// YCbCr 4:2:0 frame (NV12 / planar layout)
struct YUVFrame {
    uint32_t        width, height;
    vector<uint8_t> Y;   // luma:        width * height bytes
    vector<uint8_t> Cb;  // blue-chroma: (width/2) * (height/2) bytes
    vector<uint8_t> Cr;  // red-chroma:  (width/2) * (height/2) bytes

    YUVFrame(uint32_t w, uint32_t h)
        : width(w),
          height(h),
          Y(w * h, 128),
          Cb((w / 2) * (h / 2), 128),
          Cr((w / 2) * (h / 2), 128) {}

    size_t byteSize() const { return Y.size() + Cb.size() + Cr.size(); }
};

// RGB <-> YCbCr (ITU-R BT.601)
struct RGB {
    uint8_t r, g, b;
};

void rgbToYCbCr(const RGB& p, uint8_t& Y, uint8_t& Cb, uint8_t& Cr) {
    Y  = (uint8_t)(16 + 0.257f * p.r + 0.504f * p.g + 0.098f * p.b);
    Cb = (uint8_t)(128 - 0.148f * p.r - 0.291f * p.g + 0.439f * p.b);
    Cr = (uint8_t)(128 + 0.439f * p.r - 0.368f * p.g - 0.071f * p.b);
}

RGB yCbCrToRGB(uint8_t Y, uint8_t Cb, uint8_t Cr) {
    float y = Y - 16, cb = Cb - 128, cr = Cr - 128;
    auto  clamp = [](float v) -> uint8_t { return v < 0 ? 0 : v > 255 ? 255 : (uint8_t)v; };
    return {
        clamp(1.164f * y + 1.596f * cr),
        clamp(1.164f * y - 0.392f * cb - 0.813f * cr),
        clamp(1.164f * y + 2.017f * cb)
    };
}

// A video is a vector of frames
struct RawVideo {
    uint32_t         width, height, fps;
    vector<YUVFrame> frames;

    RawVideo(uint32_t w, uint32_t h, uint32_t f) : width(w), height(h), fps(f) {}

    double duration() const { return (double)frames.size() / fps; }
    size_t totalBytes() const { return frames.empty() ? 0 : frames[0].byteSize() * frames.size(); }
};

// Write raw YUV420 file
void writeYUV(const RawVideo& vid, const char* path) {
    // Playback: ffplay -f rawvideo -pixel_format yuv420p -video_size WxH file.yuv
    ofstream f(path, ios::binary);
    for (const auto& frame : vid.frames) {
        f.write((char*)frame.Y.data(), frame.Y.size());
        f.write((char*)frame.Cb.data(), frame.Cb.size());
        f.write((char*)frame.Cr.data(), frame.Cr.size());
    }
}

void printStats(const RawVideo& vid) {
    size_t rgb = vid.width * vid.height * 3 * vid.frames.size();
    printf("Video   : %ux%u @ %u fps\n", vid.width, vid.height, vid.fps);
    printf("Frames  : %zu   Duration: %.2fs\n", vid.frames.size(), vid.duration());
    printf("YUV raw : %.1f MB\n", vid.totalBytes() / 1e6);
    printf(
        "RGB raw : %.1f MB  (YUV saves %.0f%%)\n",
        rgb / 1e6,
        (1.0 - (double)vid.totalBytes() / rgb) * 100
    );
}

int main() {
    RawVideo vid(320, 240, 30);

    for (uint32_t f = 0; f < 90; ++f) {  // 3 seconds @ 30fps
        YUVFrame frame(320, 240);
        float    t     = (float)f / 90;
        RGB      color = {(uint8_t)(255 * t), (uint8_t)(255 * (1 - t)), 128};
        uint8_t  Y, Cb, Cr;
        rgbToYCbCr(color, Y, Cb, Cr);
        fill(frame.Y.begin(), frame.Y.end(), Y);
        fill(frame.Cb.begin(), frame.Cb.end(), Cb);
        fill(frame.Cr.begin(), frame.Cr.end(), Cr);
        vid.frames.push_back(move(frame));
    }

    printStats(vid);
    writeYUV(vid, "output.yuv");
    printf("\nWrote output.yuv\n");
    printf("Play: ffplay -f rawvideo -pixel_format yuv420p -video_size 320x240 output.yuv\n");
}
