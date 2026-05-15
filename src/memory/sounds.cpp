#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

constexpr double PI = 3.14159265358979323846;

// PCM Audio buffer: the raw memory layout of sound
struct AudioBuffer {
    uint32_t        sampleRate;     // e.g. 44100 samples per second
    uint16_t        channels;       // 1 = mono,  2 = stereo
    uint16_t        bitsPerSample;  // 8, 16, 24, or 32
    vector<int16_t> samples;        // stereo layout: L0 R0 L1 R1 L2 R2 ...

    double duration() const { return (double)samples.size() / (sampleRate * channels); }
};

// Generate a pure sine wave
AudioBuffer sineWave(double freq, double durSec, uint32_t sampleRate = 44100) {
    AudioBuffer buf;
    buf.sampleRate    = sampleRate;
    buf.channels      = 1;
    buf.bitsPerSample = 16;
    buf.samples.resize((size_t)(durSec * sampleRate));

    for (size_t i = 0; i < buf.samples.size(); ++i) {
        double t       = (double)i / sampleRate;
        double angle   = 2.0 * PI * freq * t;
        buf.samples[i] = (int16_t)(sin(angle) * 32000.0);
        // int16_t range: -32768 to +32767
    }
    return buf;
}

// Show the raw bytes of the first N samples
void inspectMemory(const AudioBuffer& buf, int n = 4) {
    printf("\nFirst %d samples (each = 2 bytes = int16_t):\n", n);
    for (int i = 0; i < n; ++i) {
        int16_t  s = buf.samples[i];
        uint8_t* b = reinterpret_cast<uint8_t*>(&s);
        printf(
            "  sample[%d] = %6d  bytes: %02X %02X  addr: %p\n",
            i,
            s,
            b[0],
            b[1],
            (void*)&buf.samples[i]
        );
    }
    printf("Total audio bytes: %zu\n", buf.samples.size() * 2);
}

// Stereo: interleave two mono buffers
AudioBuffer toStereo(const AudioBuffer& left, const AudioBuffer& right) {
    AudioBuffer out;
    out.sampleRate    = left.sampleRate;
    out.channels      = 2;
    out.bitsPerSample = 16;
    size_t n          = min(left.samples.size(), right.samples.size());
    out.samples.reserve(n * 2);
    for (size_t i = 0; i < n; ++i) {
        out.samples.push_back(left.samples[i]);   // Left channel
        out.samples.push_back(right.samples[i]);  // Right channel
    }
    // Byte layout: [L0 lo][L0 hi][R0 lo][R0 hi][L1 lo][L1 hi]...
    return out;
}

// Write a WAV file (44-byte header + raw PCM data)
void writeWAV(const AudioBuffer& buf, const char* path) {
#pragma pack(push, 1)
    struct WAVHeader {
        char     riff[4] = {'R', 'I', 'F', 'F'};
        uint32_t chunkSize;
        char     wave[4]      = {'W', 'A', 'V', 'E'};
        char     fmt[4]       = {'f', 'm', 't', ' '};
        uint32_t subChunkSize = 16;
        uint16_t audioFormat  = 1;  // 1 = PCM (uncompressed)
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;    // sampleRate * channels * bitsPerSample/8
        uint16_t blockAlign;  // channels * bitsPerSample/8
        uint16_t bitsPerSample;
        char     data[4] = {'d', 'a', 't', 'a'};
        uint32_t dataSize;
    };
#pragma pack(pop)

    uint32_t  dataBytes = buf.samples.size() * sizeof(int16_t);
    WAVHeader hdr;
    hdr.numChannels   = buf.channels;
    hdr.sampleRate    = buf.sampleRate;
    hdr.bitsPerSample = buf.bitsPerSample;
    hdr.blockAlign    = buf.channels * buf.bitsPerSample / 8;
    hdr.byteRate      = buf.sampleRate * hdr.blockAlign;
    hdr.dataSize      = dataBytes;
    hdr.chunkSize     = 36 + dataBytes;

    ofstream f(path, ios::binary);
    f.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    f.write(reinterpret_cast<const char*>(buf.samples.data()), dataBytes);
}

int main() {
    AudioBuffer tone = sineWave(440.0, 0.01);  // A-440 Hz, 10ms
    inspectMemory(tone, 4);

    // A musical chord: A4 + C#5 in stereo
    AudioBuffer left  = sineWave(440.0, 1.0);
    AudioBuffer right = sineWave(554.4, 1.0);
    AudioBuffer chord = toStereo(left, right);

    printf(
        "\nStereo: %zu samples, %zu bytes, %.2f sec\n",
        chord.samples.size(),
        chord.samples.size() * 2,
        chord.duration()
    );

    writeWAV(chord, "chord.wav");
    printf("Wrote chord.wav — open in Audacity or any audio player\n");
}
