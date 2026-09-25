#include "AxUdioCore.h"
#include <algorithm>
#include <cmath>

// AxUdio Dek — передискретизация (ресемплинг) аудиопотока до строго 44100 Гц
// Без сторонних библиотек (Native C++)
bool AxUdio_Dek::Process44100(const std::vector<float>& pcm, int currentRate) {
    resampledBuffer.clear();

    // Проверка корректности входных данных
    if (pcm.empty() || currentRate <= 0) {
        return false;
    }

    constexpr int TARGET_RATE = 44100;
    constexpr int CHANNELS = 2; // Ожидаемый стереопоток (L/R)

    // 1. Если частота уже строго 44100 Гц — применяем быстрое копирование
    if (currentRate == TARGET_RATE) {
        resampledBuffer = pcm;
        return true;
    }

    // 2. Линейный ресемплинг для адаптации под 44100 Гц
    const double ratio = static_cast<double>(currentRate) / static_cast<double>(TARGET_RATE);

    // Вычисляем количество фреймов (один фрейм = CHANNELS сэмплов)
    const size_t inputFrames = pcm.size() / CHANNELS;
    if (inputFrames == 0) return false;

    const size_t outputFrames = static_cast<size_t>(std::ceil(inputFrames / ratio));
    const size_t outputSize = outputFrames * CHANNELS;

    resampledBuffer.resize(outputSize);

    for (size_t i = 0; i < outputFrames; ++i) {
        double srcFrameIndex = i * ratio;
        size_t frameLow = static_cast<size_t>(srcFrameIndex);
        size_t frameHigh = frameLow + 1;

        // Защита от выхода за границы для последнего фрейма
        if (frameHigh >= inputFrames) {
            frameHigh = frameLow;
        }

        float fraction = static_cast<float>(srcFrameIndex - frameLow);

        // Интерполируем каждый канал отдельно (L и R)
        for (int ch = 0; ch < CHANNELS; ++ch) {
            size_t sampleLowIndex = frameLow * CHANNELS + ch;
            size_t sampleHighIndex = frameHigh * CHANNELS + ch;

            // Защитный фоллбэк на случай некорректного размера выравнивания
            if (sampleHighIndex >= pcm.size()) {
                sampleHighIndex = sampleLowIndex;
            }

            float lowSample = pcm[sampleLowIndex];
            float highSample = pcm[sampleHighIndex];

            // Формула линейной интерполяции
            resampledBuffer[i * CHANNELS + ch] = lowSample + fraction * (highSample - lowSample);
        }
    }

    return !resampledBuffer.empty();
}