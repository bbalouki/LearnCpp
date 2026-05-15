#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Packet: the indivisible unit of A/V data
enum class StreamType : uint8_t { VIDEO = 0, AUDIO = 1 };

struct Packet {
    StreamType      type;
    int64_t         pts;  // presentation timestamp (microseconds)
    int64_t         dts;  // decode timestamp (differs for B-frames)
    bool            keyframe;
    vector<uint8_t> data;  // compressed payload (H.264 NAL / AAC frame)

    size_t serializedSize() const { return 22 + data.size(); }
    // 22 = 1 type + 8 pts + 8 dts + 1 flags + 4 size
};

// Muxer: collect and interleave A/V packets
struct AVMuxer {
    vector<Packet> packets;

    void addVideo(int64_t pts, bool key, vector<uint8_t> payload) {
        packets.push_back({StreamType::VIDEO, pts, pts, key, move(payload)});
    }
    void addAudio(int64_t pts, vector<uint8_t> payload) {
        packets.push_back({StreamType::AUDIO, pts, pts, true, move(payload)});
    }
    // Sort all packets by decode timestamp before writing
    void finalize() {
        sort(packets.begin(), packets.end(), [](const Packet& a, const Packet& b) {
            return a.dts < b.dts;
        });
    }
    void printTable() const {
        size_t total = 0;
        printf("\n%-10s %-10s %-6s %-10s\n", "TYPE", "PTS(ms)", "KEY", "BYTES");
        puts("──────────────────────────────────────");
        for (const auto& p : packets) {
            printf(
                "%-10s %-10lld %-6s %-10zu\n",
                p.type == StreamType::VIDEO ? "VIDEO" : "AUDIO",
                (long long)(p.pts / 1000),
                p.keyframe ? "yes" : "no",
                p.serializedSize()
            );
            total += p.serializedSize();
        }
        printf("\nTotal container size: %zu bytes\n", total);
    }
};

// Demuxer: split container back into separate streams
struct AVDemuxer {
    vector<Packet> video, audio;
    void           demux(const vector<Packet>& src) {
        for (const auto& p : src) (p.type == StreamType::VIDEO ? video : audio).push_back(p);
    }
};

// A/V sync: keep video and audio aligned during playback
struct AVClock {
    int64_t videoPTS = 0, audioPTS = 0;
    void    onVideo(int64_t pts) { videoPTS = pts; }
    void    onAudio(int64_t pts) { audioPTS = pts; }
    int64_t videoLead() const { return videoPTS - audioPTS; }          // microseconds
    bool    shouldDelay() const { return videoLead() > 50000; }        // video > 50ms ahead
    bool    shouldDropFrame() const { return videoLead() < -100000; }  // video > 100ms behind
};

// Write a minimal container file
void writeMuxed(const AVMuxer& mux, const char* path) {
    ofstream   f(path, ios::binary);
    const char magic[] = "AVMUX";
    f.write(magic, 5);
    uint32_t count = mux.packets.size();
    f.write((char*)&count, 4);
    for (const auto& p : mux.packets) {
        uint8_t  type  = (uint8_t)p.type;
        uint8_t  flags = p.keyframe ? 1 : 0;
        uint32_t sz    = p.data.size();
        f.write((char*)&type, 1);
        f.write((char*)&p.pts, 8);
        f.write((char*)&p.dts, 8);
        f.write((char*)&flags, 1);
        f.write((char*)&sz, 4);
        f.write((char*)p.data.data(), sz);
    }
}

int main() {
    AVMuxer mux;
    int64_t usPerFrame = 33333;  // 30 fps = 33.3ms/frame
    int64_t usPerAudio = 23220;  // AAC = 1024 samples @ 44100 Hz

    for (int i = 0; i < 10; ++i) {
        bool key = (i % 3 == 0);  // keyframe every 3rd frame (GOP size=3)
        // I-frames are ~10x larger than P-frames
        mux.addVideo(i * usPerFrame, key, vector<uint8_t>(key ? 8000 : 800, 0xAB));
    }
    for (int i = 0; i < 14; ++i) mux.addAudio(i * usPerAudio, vector<uint8_t>(384, 0xCD));

    mux.finalize();
    mux.printTable();
    writeMuxed(mux, "output.avmux");

    AVDemuxer demux;
    demux.demux(mux.packets);
    printf(
        "\nDemuxed -> Video: %zu pkts  Audio: %zu pkts\n", demux.video.size(), demux.audio.size()
    );

    AVClock clock;
    clock.onAudio(100000);
    clock.onVideo(200000);  // video 100ms ahead of audio
    printf(
        "Video lead: %lldms  Should delay video: %s\n",
        (long long)(clock.videoLead() / 1000),
        clock.shouldDelay() ? "yes" : "no"
    );
}
