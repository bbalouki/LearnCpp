#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

// Packet: the indivisible unit of A/V data
enum class StreamType : uint8_t { VIDEO = 0, AUDIO = 1 };

struct Packet {
    StreamType           type;
    int64_t              pts;  // presentation timestamp (microseconds)
    int64_t              dts;  // decode timestamp (differs for B-frames)
    bool                 keyframe;
    std::vector<uint8_t> data;  // compressed payload (H.264 NAL / AAC frame)

    size_t serializedSize() const {
        constexpr size_t headerSize = 22;
        return headerSize + data.size();
    }
    // 22 = 1 type + 8 pts + 8 dts + 1 flags + 4 size
};

// Muxer: collect and interleave A/V packets
struct AVMuxer {
    std::vector<Packet> packets;

    void addVideo(int64_t pts, bool key, std::vector<uint8_t> payload) {
        packets.push_back({StreamType::VIDEO, pts, pts, key, std::move(payload)});
    }
    void addAudio(int64_t pts, std::vector<uint8_t> payload) {
        packets.push_back({StreamType::AUDIO, pts, pts, true, std::move(payload)});
    }
    // Sort all packets by decode timestamp before writing
    void finalize() {
        std::sort(packets.begin(), packets.end(), [](const Packet& pktA, const Packet& pktB) {
            return pktA.dts < pktB.dts;
        });
    }
    void printTable() const {
        size_t totalBytes = 0;
        std::println("\n{:<10} {:<10} {:<6} {:<10}", "TYPE", "PTS(ms)", "KEY", "BYTES");
        std::println("──────────────────────────────────────");
        for (const auto& packet : packets) {
            constexpr int msDivisor = 1000;
            std::println(
                "{:<10} {:<10} {:<6} {:<10}",
                packet.type == StreamType::VIDEO ? "VIDEO" : "AUDIO",
                static_cast<long long>(packet.pts / msDivisor),
                packet.keyframe ? "yes" : "no",
                packet.serializedSize()
            );
            totalBytes += packet.serializedSize();
        }
        std::println("\nTotal container size: {} bytes", totalBytes);
    }
};

// Demuxer: split container back into separate streams
struct AVDemuxer {
    std::vector<Packet> video, audio;
    void                demux(const std::vector<Packet>& source) {
        for (const auto& packet : source)
            (packet.type == StreamType::VIDEO ? video : audio).push_back(packet);
    }
};

// A/V sync: keep video and audio aligned during playback
struct AVClock {
    int64_t videoPTS = 0, audioPTS = 0;
    void    onVideo(int64_t pts) { videoPTS = pts; }
    void    onAudio(int64_t pts) { audioPTS = pts; }
    int64_t videoLead() const { return videoPTS - audioPTS; }  // microseconds
    bool    shouldDelay() const {
        constexpr int delayThreshold = 50000;  // 50ms
        return videoLead() > delayThreshold;
    }
    bool shouldDropFrame() const {
        constexpr int dropThreshold = -100000;  // 100ms
        return videoLead() < dropThreshold;
    }
};

// Write a minimal container file
void writeMuxed(const AVMuxer& mux, const char* filePath) {
    std::ofstream    file(filePath, std::ios::binary);
    const char       magic[]   = "AVMUX";
    constexpr size_t magicSize = 5;
    file.write(magic, magicSize);
    uint32_t count = static_cast<uint32_t>(mux.packets.size());
    file.write(reinterpret_cast<char*>(&count), sizeof(uint32_t));
    for (const auto& packet : mux.packets) {
        uint8_t  type     = static_cast<uint8_t>(packet.type);
        uint8_t  flags    = packet.keyframe ? 1 : 0;
        uint32_t dataSize = static_cast<uint32_t>(packet.data.size());
        file.write(reinterpret_cast<char*>(&type), sizeof(uint8_t));
        file.write(reinterpret_cast<const char*>(&packet.pts), sizeof(int64_t));
        file.write(reinterpret_cast<const char*>(&packet.dts), sizeof(int64_t));
        file.write(reinterpret_cast<char*>(&flags), sizeof(uint8_t));
        file.write(reinterpret_cast<char*>(&dataSize), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(packet.data.data()), dataSize);
    }
}

int main() {
    AVMuxer           mux;
    constexpr int64_t usPerFrame = 33333;  // 30 fps = 33.3ms/frame
    constexpr int64_t usPerAudio = 23220;  // AAC = 1024 samples @ 44100 Hz

    constexpr int videoPacketCount = 10;
    for (int index = 0; index < videoPacketCount; ++index) {
        constexpr int keyframeInterval = 3;
        bool key = (index % keyframeInterval == 0);  // keyframe every 3rd frame (GOP size=3)
        // I-frames are ~10x larger than P-frames
        constexpr size_t keyframeSize = 8000;
        constexpr size_t pFrameSize   = 800;
        mux.addVideo(
            index * usPerFrame, key, std::vector<uint8_t>(key ? keyframeSize : pFrameSize, 0xAB)
        );
    }
    constexpr int audioPacketCount = 14;
    for (int index = 0; index < audioPacketCount; ++index) {
        constexpr size_t audioPayloadSize = 384;
        mux.addAudio(index * usPerAudio, std::vector<uint8_t>(audioPayloadSize, 0xCD));
    }

    mux.finalize();
    mux.printTable();
    writeMuxed(mux, "output.avmux");

    AVDemuxer demux;
    demux.demux(mux.packets);
    std::println(
        "\nDemuxed -> Video: {} pkts  Audio: {} pkts", demux.video.size(), demux.audio.size()
    );

    AVClock           clock;
    constexpr int64_t audioStartPTS = 100000;
    constexpr int64_t videoStartPTS = 200000;
    clock.onAudio(audioStartPTS);
    clock.onVideo(videoStartPTS);  // video 100ms ahead of audio
    constexpr int msDivisor = 1000;
    std::println(
        "Video lead: {}ms  Should delay video: {}",
        static_cast<long long>(clock.videoLead() / msDivisor),
        clock.shouldDelay() ? "yes" : "no"
    );
}
