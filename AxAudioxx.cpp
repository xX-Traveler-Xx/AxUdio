#include "AxUdioCore.h"
#include <algorithm>
#include <cstring>

#if defined(__ANDROID__)
#include <aaudio/AAudio.h>
#include <android/log.h>

#define LOG_TAG "AxUdioEngine"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static AAudioStream* g_audioStream = nullptr;
static std::vector<float> g_ringBuffer;
static size_t g_readIndex = 0;
static bool g_protectionEnabled = true;

// Лимитер защиты
static float ApplyProtection(float sample) {
    if (!g_protectionEnabled) return sample;
    const float threshold = 0.7f;
    if (sample > threshold) {
        return threshold + (sample - threshold) / (1.0f + (sample - threshold));
    }
    else if (sample < -threshold) {
        return -threshold + (sample + threshold) / (1.0f - (sample + threshold));
    }
    return sample;
}

// Колбэк AAudio
static aaudio_data_callback_result_t AudioCallback(
    AAudioStream* stream, void* userData,
    void* audioData, int32_t numFrames) {

    float* outputBuffer = static_cast<float*>(audioData);

    for (int i = 0; i < numFrames; ++i) {
        float sampleLeft = 0.0f;
        float sampleRight = 0.0f;

        if (!g_ringBuffer.empty() && g_readIndex + 1 < g_ringBuffer.size()) {
            sampleLeft = g_ringBuffer[g_readIndex++];
            sampleRight = g_ringBuffer[g_readIndex++];
        }

        *outputBuffer++ = ApplyProtection(sampleLeft);
        *outputBuffer++ = ApplyProtection(sampleRight);
    }

    // Очистка памяти кольцевого буфера
    if (g_readIndex > 384000) {
        if (g_readIndex >= g_ringBuffer.size()) {
            g_ringBuffer.clear();
            g_readIndex = 0;
        }
        else {
            g_ringBuffer.erase(g_ringBuffer.begin(), g_ringBuffer.begin() + g_readIndex);
            g_readIndex = 0;
        }
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

bool AxUdio_AxAudioxx::OutputToHardware(const std::vector<float>& buffer) {
    if (buffer.empty()) return false;

    // Инициализация потока AAudio
    if (g_audioStream == nullptr) {
        AAudioStreamBuilder* builder = nullptr;
        if (AAudio_createStreamBuilder(&builder) != AAUDIO_OK) return false;

        AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_EXCLUSIVE);
        AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
        AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
        AAudioStreamBuilder_setChannelCount(builder, 2);
        AAudioStreamBuilder_setDataCallback(builder, AudioCallback, nullptr);

        aaudio_result_t result = AAudioStreamBuilder_openStream(builder, &g_audioStream);
        if (result == AAUDIO_OK && g_audioStream != nullptr) {
            AAudioStream_requestStart(g_audioStream);
        }
        else {
            LOGE("Не удалось открыть AAudio поток");
            AAudioStreamBuilder_delete(builder);
            return false;
        }
        AAudioStreamBuilder_delete(builder);
    }

    // Сброс прочитанных данных при заполнении
    if (g_readIndex >= g_ringBuffer.size() && !g_ringBuffer.empty()) {
        g_ringBuffer.clear();
        g_readIndex = 0;
    }

    // Добавление новых сэмплов в кольцевой буфер
    size_t oldSize = g_ringBuffer.size();
    g_ringBuffer.resize(oldSize + buffer.size());
    std::memcpy(&g_ringBuffer[oldSize], buffer.data(), buffer.size() * sizeof(float));

    return true;
}

#elif defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

bool AxUdio_AxAudioxx::OutputToHardware(const std::vector<float>& buffer) {
    if (buffer.empty()) return false;

    static HWAVEOUT hWaveOut = NULL;

    if (hWaveOut == NULL) {
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 2, 44100, 44100 * 4, 4, 16, 0 };
        if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
            return false;
        }
    }

    std::vector<int16_t> pcm16(buffer.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
        pcm16[i] = static_cast<int16_t>(std::clamp(buffer[i], -1.0f, 1.0f) * 32767.0f);
    }

    WAVEHDR header = {};
    header.lpData = reinterpret_cast<LPSTR>(pcm16.data());
    header.dwBufferLength = static_cast<DWORD>(pcm16.size() * sizeof(int16_t));

    if (waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR)) == MMSYSERR_NOERROR) {
        waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
        while ((header.dwFlags & WHDR_DONE) == 0) {
            Sleep(1);
        }
        waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        return true;
    }
    return false;
}
#endif

bool AxUdio_audioxcard::SendToAxAudioxx(const std::vector<float>& buffer) {
    return driver.OutputToHardware(buffer);
}