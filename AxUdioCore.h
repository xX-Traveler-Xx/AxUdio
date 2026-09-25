#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <algorithm> // Обязательно для std::clamp и std::min/std::max

#ifdef AXUDIO_EXPORTS
#define AXUDIO_API __declspec(dllexport)
#else
#define AXUDIO_API __declspec(dllimport)
#endif

// Структура для обмена данными с C#
struct AnalysisData {
    int bitrateKbps;
    float peakVolume;
    float rmsVolume;
    int isPlaying; // 1 — играет, 0 — конец трека
};

// Объявления классов подсистем, используемые в .cpp файлах

class AxUdio_AxAudioxx {
public:
    bool OutputToHardware(const std::vector<float>& buffer);
};

class AxUdio_audioxcard {
public:
    AxUdio_AxAudioxx driver;
    bool SendToAxAudioxx(const std::vector<float>& buffer);
};

class AxUdio_bxxitAnalysis {
public:
    static AnalysisData Analyze(const std::vector<float>& pcm, size_t fileSize);
};

class AxUdio_Dek {
public:
    int targetRate = 44100;
    std::vector<float> resampledBuffer;
    bool Process44100(const std::vector<float>& pcm, int currentRate);
};

class AxUdio_SO {
public:
    std::vector<uint8_t> ramBuffer;
    bool LoadToRAM(const uint8_t* fileData, size_t size);
};

class AxUdio_ZxZipl {
public:
    std::vector<float> pcmBuffer;
    bool DecompressStream(const std::vector<uint8_t>& rawBuffer);
};


// C-обертка для экспорта в C#
extern "C" {
    AXUDIO_API void* AxUdio_Create();
    AXUDIO_API void AxUdio_Destroy(void* instance);

    AXUDIO_API bool AxUdio_OpenStream(void* instance, const unsigned char* fileBytes, size_t size);
    AXUDIO_API bool AxUdio_ReadNextChunk(void* instance);
    AXUDIO_API AnalysisData AxUdio_GetAnalysis(void* instance);
}