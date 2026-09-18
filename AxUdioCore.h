#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>

#if defined(_WIN32)
#define AX_EXPORT __declspec(dllexport)
#else
#define AX_EXPORT __attribute__((visibility("default")))
#endif

struct AxUdio_AnalysisData {
    int bitrateKbps;
    float peakVolume;
    float rmsVolume;
};

class AxUdio_SO {
public:
    std::vector<uint8_t> ramBuffer;
    bool LoadToRAM(const uint8_t* fileData, size_t size);
};

class AxUdio_ZxZipl {
public:
    std::vector<float> pcmBuffer;
    int sourceSampleRate = 48000;
    bool DecompressStream(const std::vector<uint8_t>& rawBuffer);
};

class AxUdio_Dek {
public:
    std::vector<float> resampledBuffer;
    const int targetRate = 44100;
    bool Process44100(const std::vector<float>& pcm, int currentRate);
};

class AxUdio_bxxitAnalysis {
public:
    static AxUdio_AnalysisData Analyze(const std::vector<float>& pcm, size_t fileSize);
};

class AxUdio_AxAudioxx {
public:
    bool OutputToHardware(const std::vector<float>& buffer);
};

class AxUdio_audioxcard {
private:
    AxUdio_AxAudioxx driver;
public:
    bool SendToAxAudioxx(const std::vector<float>& buffer);
};