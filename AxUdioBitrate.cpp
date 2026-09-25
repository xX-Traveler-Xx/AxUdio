#include "AxUdioCore.h"
#include <algorithm>
#include <cmath>

// Реализация метода анализа сигнала
AnalysisData AxUdio_bxxitAnalysis::Analyze(const std::vector<float>& pcm, size_t fileSize) {
    AnalysisData data{};

    if (pcm.empty()) {
        data.bitrateKbps = 0;
        data.peakVolume = 0.0f;
        data.rmsVolume = 0.0f;
        data.isPlaying = 0;
        return data;
    }

    float maxVal = 0.0f;
    float sumSquare = 0.0f;

    for (float sample : pcm) {
        float absSample = std::fabs(sample);
        if (absSample > maxVal) {
            maxVal = absSample;
        }
        sumSquare += sample * sample;
    }

    data.peakVolume = maxVal;
    data.rmsVolume = std::sqrt(sumSquare / static_cast<float>(pcm.size()));

    // Примерная оценка битрейта на основе размера и длительности
    if (fileSize > 0) {
        data.bitrateKbps = static_cast<int>((fileSize * 8) / 1000);
    }
    else {
        data.bitrateKbps = 320;
    }

    data.isPlaying = 1;
    return data;
}

// ВАЖНО: Удалите из этого файла все экспортные функции:
// AxUdio_Create, AxUdio_Destroy, AxUdio_GetAnalysis, AxUdio_OpenStream, AxUdio_ReadNextChunk.
// Они должны находиться ТОЛЬКО в AxUdioPipeline.cpp.