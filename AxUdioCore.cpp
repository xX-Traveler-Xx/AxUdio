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

struct SongInfo {
    char title[256];
    char artist[256];
    char album[256];
    uint8_t* pictureData;
    uint32_t pictureSize;
};

struct AxUdio_AnalysisData {
    int bitrateKbps;
    float peakVolume;
    float rmsVolume;
};