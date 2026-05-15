#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

constexpr double PI_CONST = 3.14159265358979323846;

// PCM Audio buffer: the raw memory layout of sound
struct AudioBuffer {
    uint32_t             sampleRate;     // e.g. 44100 samples per second
    uint16_t             channels;       // 1 = mono,  2 = stereo
    uint16_t             bitsPerSample;  // 8, 16, 24, or 32
    std::vector<int16_t> samples;        // stereo layout: L0 R0 L1 R1 L2 R2 ...

    double duration() const {
        return static_cast<double>(samples.size()) / (sampleRate * channels);
    }
};

// Generate a pure sine wave
AudioBuffer sineWave(double frequency, double durationSec, uint32_t sampleRate = 44100) {
    AudioBuffer buffer;
    buffer.sampleRate    = sampleRate;
    buffer.channels      = 1;
    buffer.bitsPerSample = 16;
    buffer.samples.resize(static_cast<size_t>(durationSec * sampleRate));

    for (size_t index = 0; index < buffer.samples.size(); ++index) {
        double           timeValue = static_cast<double>(index) / sampleRate;
        double           angle     = 2.0 * PI_CONST * frequency * timeValue;
        constexpr double amplitude = 32000.0;
        buffer.samples[index]      = static_cast<int16_t>(std::sin(angle) * amplitude);
        // int16_t range: -32768 to +32767
    }
    return buffer;
}

// Show the raw bytes of the first N samples
void inspectMemory(const AudioBuffer& buffer, int count = 4) {
    std::println("\nFirst {} samples (each = 2 bytes = int16_t):", count);
    for (int index = 0; index < count; ++index) {
        int16_t  sample  = buffer.samples[index];
        uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&sample);
        std::println(
            "  sample[{}] = {:6}  bytes: {:02X} {:02X}  addr: {}",
            index,
            sample,
            bytePtr[0],
            bytePtr[1],
            static_cast<const void*>(&buffer.samples[index])
        );
    }
    constexpr size_t bytesPerSample = 2;
    std::println("Total audio bytes: {}", buffer.samples.size() * bytesPerSample);
}

// Stereo: interleave two mono buffers
AudioBuffer toStereo(const AudioBuffer& left, const AudioBuffer& right) {
    AudioBuffer output;
    output.sampleRate           = left.sampleRate;
    output.channels             = 2;
    output.bitsPerSample        = 16;
    size_t        count         = std::min(left.samples.size(), right.samples.size());
    constexpr int channelsCount = 2;
    output.samples.reserve(count * channelsCount);
    for (size_t index = 0; index < count; ++index) {
        output.samples.push_back(left.samples[index]);   // Left channel
        output.samples.push_back(right.samples[index]);  // Right channel
    }
    // Byte layout: [L0 lo][L0 hi][R0 lo][R0 hi][L1 lo][L1 hi]...
    return output;
}

// Write a WAV file (44-byte header + raw PCM data)
void writeWAV(const AudioBuffer& buffer, const char* filePath) {
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

    uint32_t  dataBytes = static_cast<uint32_t>(buffer.samples.size() * sizeof(int16_t));
    WAVHeader header;
    header.numChannels        = buffer.channels;
    header.sampleRate         = buffer.sampleRate;
    header.bitsPerSample      = buffer.bitsPerSample;
    constexpr int bitsPerByte = 8;
    header.blockAlign = static_cast<uint16_t>(buffer.channels * buffer.bitsPerSample / bitsPerByte);
    header.byteRate   = buffer.sampleRate * header.blockAlign;
    header.dataSize   = dataBytes;
    constexpr uint32_t headerOffset = 36;
    header.chunkSize                = headerOffset + dataBytes;

    std::ofstream file(filePath, std::ios::binary);
    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    file.write(reinterpret_cast<const char*>(buffer.samples.data()), dataBytes);
}

int main() {
    constexpr double freqA440     = 440.0;
    constexpr double duration10ms = 0.01;
    AudioBuffer      tone         = sineWave(freqA440, duration10ms);  // A-440 Hz, 10ms
    inspectMemory(tone, 4);

    // A musical chord: A4 + C#5 in stereo
    constexpr double duration1s  = 1.0;
    constexpr double freqCSharp5 = 554.4;
    AudioBuffer      left        = sineWave(freqA440, duration1s);
    AudioBuffer      right       = sineWave(freqCSharp5, duration1s);
    AudioBuffer      chord       = toStereo(left, right);

    constexpr size_t bytesPerSample = 2;
    std::println(
        "\nStereo: {} samples, {} bytes, {:.2f} sec",
        chord.samples.size(),
        chord.samples.size() * bytesPerSample,
        chord.duration()
    );

    writeWAV(chord, "chord.wav");
    std::println("Wrote chord.wav — open in Audacity or any audio player");
}
