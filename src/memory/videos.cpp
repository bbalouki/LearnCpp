#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

// YCbCr 4:2:0 frame (NV12 / planar layout)
struct YUVFrame {
    uint32_t             width, height;
    std::vector<uint8_t> luma;     // Y:        width * height bytes
    std::vector<uint8_t> chromaB;  // Cb: (width/2) * (height/2) bytes
    std::vector<uint8_t> chromaR;  // Cr:  (width/2) * (height/2) bytes

    YUVFrame(uint32_t widthVal, uint32_t heightVal)
        : width(widthVal),
          height(heightVal),
          luma(widthVal * heightVal, 128),
          chromaB((widthVal / 2) * (heightVal / 2), 128),
          chromaR((widthVal / 2) * (heightVal / 2), 128) {}

    size_t byteSize() const { return luma.size() + chromaB.size() + chromaR.size(); }
};

// RGB <-> YCbCr (ITU-R BT.601)
struct RGB {
    uint8_t red, green, blue;
};

void rgbToYCbCr(const RGB& pixel, uint8_t& luma, uint8_t& chromaB, uint8_t& chromaR) {
    constexpr float lumaBase   = 16.0f;
    constexpr float chromaBase = 128.0f;
    luma                       = static_cast<uint8_t>(
        lumaBase + 0.257f * pixel.red + 0.504f * pixel.green + 0.098f * pixel.blue
    );
    chromaB = static_cast<uint8_t>(
        chromaBase - 0.148f * pixel.red - 0.291f * pixel.green + 0.439f * pixel.blue
    );
    chromaR = static_cast<uint8_t>(
        chromaBase + 0.439f * pixel.red - 0.368f * pixel.green - 0.071f * pixel.blue
    );
}

RGB yCbCrToRGB(uint8_t luma, uint8_t chromaB, uint8_t chromaR) {
    constexpr float lumaOffset   = 16.0f;
    constexpr float chromaOffset = 128.0f;
    float yVal = luma - lumaOffset, cbVal = chromaB - chromaOffset, crVal = chromaR - chromaOffset;
    auto  clamp = [](float val) -> uint8_t {
        constexpr float maxVal = 255.0f;
        return val < 0 ? 0 : val > maxVal ? 255 : static_cast<uint8_t>(val);
    };
    return {
        clamp(1.164f * yVal + 1.596f * crVal),
        clamp(1.164f * yVal - 0.392f * cbVal - 0.813f * crVal),
        clamp(1.164f * yVal + 2.017f * cbVal)
    };
}

// A video is a vector of frames
struct RawVideo {
    uint32_t              width, height, fps;
    std::vector<YUVFrame> frames;

    RawVideo(uint32_t widthVal, uint32_t heightVal, uint32_t fpsVal)
        : width(widthVal), height(heightVal), fps(fpsVal) {}

    double duration() const { return static_cast<double>(frames.size()) / fps; }
    size_t totalBytes() const { return frames.empty() ? 0 : frames[0].byteSize() * frames.size(); }
};

// Write raw YUV420 file
void writeYUV(const RawVideo& video, const char* filePath) {
    // Playback: ffplay -f rawvideo -pixel_format yuv420p -video_size WxH file.yuv
    std::ofstream file(filePath, std::ios::binary);
    for (const auto& frame : video.frames) {
        file.write(reinterpret_cast<const char*>(frame.luma.data()), frame.luma.size());
        file.write(reinterpret_cast<const char*>(frame.chromaB.data()), frame.chromaB.size());
        file.write(reinterpret_cast<const char*>(frame.chromaR.data()), frame.chromaR.size());
    }
}

void printStats(const RawVideo& video) {
    constexpr int bytesPerPixelRGB = 3;
    size_t        rgbTotal = video.width * video.height * bytesPerPixelRGB * video.frames.size();
    std::println("Video   : {}x{} @ {} fps", video.width, video.height, video.fps);
    std::println("Frames  : {}   Duration: {:.2f}s", video.frames.size(), video.duration());
    constexpr double MB_DIVISOR = 1e6;
    std::println("YUV raw : {:.1f} MB", video.totalBytes() / MB_DIVISOR);
    std::println(
        "RGB raw : {:.1f} MB  (YUV saves {:.0f}%)",
        rgbTotal / MB_DIVISOR,
        (1.0 - static_cast<double>(video.totalBytes()) / rgbTotal) * 100
    );
}

int main() {
    constexpr uint32_t width  = 320;
    constexpr uint32_t height = 240;
    constexpr uint32_t fps    = 30;
    RawVideo           video(width, height, fps);

    constexpr int totalFrames = 90;
    for (uint32_t index = 0; index < totalFrames; ++index) {  // 3 seconds @ 30fps
        YUVFrame          frame(width, height);
        float             timeFactor = static_cast<float>(index) / totalFrames;
        constexpr uint8_t maxColor   = 255;
        constexpr uint8_t halfColor  = 128;
        RGB               color      = {
            static_cast<uint8_t>(maxColor * timeFactor),
            static_cast<uint8_t>(maxColor * (1.0f - timeFactor)),
            halfColor
        };
        uint8_t luma, chromaB, chromaR;
        rgbToYCbCr(color, luma, chromaB, chromaR);
        std::fill(frame.luma.begin(), frame.luma.end(), luma);
        std::fill(frame.chromaB.begin(), frame.chromaB.end(), chromaB);
        std::fill(frame.chromaR.begin(), frame.chromaR.end(), chromaR);
        video.frames.push_back(std::move(frame));
    }

    printStats(video);
    writeYUV(video, "output.yuv");
    std::println("\nWrote output.yuv");
    std::println("Play: ffplay -f rawvideo -pixel_format yuv420p -video_size 320x240 output.yuv");
}
