// AxUdioDek.cpp
#include "AxUdioCore.h"
#include <algorithm>
#include <cmath>

// Кроссплатформенная обработка и ресемплинг аудиоданных
bool AxUdio_Dek::Process44100(const std::vector<float>& pcm, int currentRate) {
    resampledBuffer.clear();

    if (pcm.empty() || currentRate <= 0) {
        return false;
    }

    constexpr int targetRate = 44100;

    // 1. Если частота уже 44100 Гц — выполняем быстрое прямым копированием (без пересчета)
    if (currentRate == targetRate) {
        resampledBuffer = pcm; // Внутри C++11 std::vector выполняет быструю memcpy-операцию
        return true;
    }

    // 2. Если частота отличается — выполняем линейный ресемплинг (подходит для Android, Linux и Windows)
    const double ratio = static_cast<double>(currentRate) / static_cast<double>(targetRate);
    const size_t outputSize = static_cast<size_t>(std::ceil(pcm.size() / ratio));

    resampledBuffer.resize(outputSize);

    for (size_t i = 0; i < outputSize; ++i) {
        double srcIndex = i * ratio;
        size_t indexLow = static_cast<size_t>(srcIndex);
        size_t indexHigh = indexLow + 1;

        // Защита от выхода за границы массива
        if (indexHigh >= pcm.size()) {
            indexHigh = indexLow;
        }

        float fraction = static_cast<float>(srcIndex - indexLow);

        // Линейная интерполяция между двумя соседними сэмплами
        resampledBuffer[i] = pcm[indexLow] + fraction * (pcm[indexHigh] - pcm[indexLow]);
    }

    return !resampledBuffer.empty();
}