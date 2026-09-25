//Важно для компиляции на Linux
//Когда будете собирать проект на Linux(например, через g++ или CMake), вам нужно будет указать флаг линковки нативной звуковой подсистемы - lasound.Это не скачиваемая из интернета библиотека, она встроена в ядро всех дистрибутивов(Ubuntu, Debian, Arch и т.д.) :
//    g++ - shared - fPIC AxAudioxx.cpp ... - lasound - o libAxUdioEngine.so


#include "AxUdioCore.h"
#include <algorithm>
#include <cstring>
#include <vector>

// =========================================================
// 1. ANDROID (AAudio)
// =========================================================
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

// =========================================================
// 2. LINUX (ALSA - Advanced Linux Sound Architecture)
// =========================================================
#elif defined(__linux__)
#include <alsa/asoundlib.h>

bool AxUdio_AxAudioxx::OutputToHardware(const std::vector<float>& buffer) {
    if (buffer.empty()) return false;

    static snd_pcm_t* pcm_handle = nullptr;

    // Однократная инициализация аудиоустройства Linux (ALSA)
    if (pcm_handle == nullptr) {
        if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
            return false;
        }
        // Настройка формата: 16-bit, Стерео, 44100 Гц, задержка 50ms
        snd_pcm_set_params(pcm_handle,
            SND_PCM_FORMAT_S16_LE,
            SND_PCM_ACCESS_RW_INTERLEAVED,
            2,
            44100,
            1,
            50000);
    }

    // Конвертация float в int16_t (безопасно для всех звуковых карт Linux)
    std::vector<int16_t> pcm16(buffer.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
        pcm16[i] = static_cast<int16_t>(std::clamp(buffer[i], -1.0f, 1.0f) * 32767.0f);
    }

    // Отправка данных на звуковую карту
    snd_pcm_sframes_t frames = snd_pcm_writei(pcm_handle, pcm16.data(), buffer.size() / 2);

    // Если буфер опустел (underrun), восстанавливаем поток
    if (frames < 0) {
        snd_pcm_recover(pcm_handle, frames, 0);
    }

    return true;
}

// =========================================================
// 3. WINDOWS (WinMM API)
// =========================================================
#elif defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

bool AxUdio_AxAudioxx::OutputToHardware(const std::vector<float>& buffer) {
    if (buffer.empty()) return false;

    static HWAVEOUT hWaveOut = NULL;
    static const int NUM_BUFFERS = 3;
    static WAVEHDR headers[NUM_BUFFERS] = {};
    static std::vector<int16_t> pcmBuffers[NUM_BUFFERS];
    static int currentBuffer = 0;

    // Однократная инициализация
    if (hWaveOut == NULL) {
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 2, 44100, 44100 * 4, 4, 16, 0 };
        if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
            return false;
        }
    }

    // Ждем, пока освободится конкретный буфер (без общего блокирования потока)
    if (headers[currentBuffer].dwFlags & WHDR_PREPARED) {
        while (!(headers[currentBuffer].dwFlags & WHDR_DONE)) {
            Sleep(1);
        }
        waveOutUnprepareHeader(hWaveOut, &headers[currentBuffer], sizeof(WAVEHDR));
    }

    // Конвертация float -> int16
    pcmBuffers[currentBuffer].resize(buffer.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
        pcmBuffers[currentBuffer][i] = static_cast<int16_t>(std::clamp(buffer[i], -1.0f, 1.0f) * 32767.0f);
    }

    WAVEHDR& hdr = headers[currentBuffer];
    ZeroMemory(&hdr, sizeof(WAVEHDR));
    hdr.lpData = reinterpret_cast<LPSTR>(pcmBuffers[currentBuffer].data());
    hdr.dwBufferLength = static_cast<DWORD>(pcmBuffers[currentBuffer].size() * sizeof(int16_t));

    // Отправка в драйвер
    if (waveOutPrepareHeader(hWaveOut, &hdr, sizeof(WAVEHDR)) == MMSYSERR_NOERROR) {
        waveOutWrite(hWaveOut, &hdr, sizeof(WAVEHDR));
        currentBuffer = (currentBuffer + 1) % NUM_BUFFERS; // Переключаем буфер по кольцу
        return true;
    }
    return false;
}
#endif

// =========================================================
// СВЯЗУЮЩЕЕ ЗВЕНО (Вызывается из других частей кода)
// =========================================================
bool AxUdio_audioxcard::SendToAxAudioxx(const std::vector<float>& buffer) {
    return driver.OutputToHardware(buffer);
}