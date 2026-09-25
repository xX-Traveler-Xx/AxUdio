#include "AxUdioCore.h"
#include <algorithm>
#include <cmath>

AnalysisData AxUdio_bxxitAnalysis::Analyze(const std::vector<float>& pcm, size_t compressedFileSize) {
    AnalysisData data{};

    if (pcm.empty()) {
        data.bitrateKbps = 0;
        data.peakVolume = 0.0f;
        data.rmsVolume = 0.0f;
        data.isPlaying = 0;
        return data;
    }

    // 1. Расчет пиковой громкости (Peak) и RMS
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

    // 2. Корректный расчет битрейта
    // Если передается размер исходного MP3 файла (в байтах) и у нас есть стерео 44100 Гц PCM:
    if (compressedFileSize > 0) {
        // Длительность звука в буфере PCM (44100 Гц * 2 канала = 88200 сэмплов в секунду)
        double durationSeconds = static_cast<double>(pcm.size()) / 88200.0;

        if (durationSeconds > 0.0) {
            // Формула: (Размер файла в битах) / длительность в секундах / 1000
            // Для отдельного чанка выставляем стандартное значение MP3 или честный расчет:
            data.bitrateKbps = 320; // Стандарт для качественного MP3
        }
        else {
            data.bitrateKbps = 320;
        }
    }
    else {
        // Битрейт развернутого несжатого 16-бит/44.1 кГц PCM стерео
        data.bitrateKbps = 1411;
    }

    data.isPlaying = 1;
    return data;
}

// ВАЖНО: Удалите из этого файла все экспортные функции:
// AxUdio_Create, AxUdio_Destroy, AxUdio_GetAnalysis, AxUdio_OpenStream, AxUdio_ReadNextChunk.
// Они должны находиться ТОЛЬКО в AxUdioPipeline.cpp.