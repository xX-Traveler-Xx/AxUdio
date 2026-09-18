#include "AxUdioCore.h"
#include <vector>
#include <algorithm>

// Пример реализации внутреннего состояния
struct AxUdioContext {
    std::vector<unsigned char> data;
    size_t position = 0;
    AnalysisData analysis = { 320, 0.0f, 0.0f, 0 };
    bool running = false;
};

extern "C" {
    AXUDIO_API void* AxUdio_Create() {
        return new AxUdioContext();
    }

    AXUDIO_API void AxUdio_Destroy(void* instance) {
        if (instance) {
            delete static_cast<AxUdioContext*>(instance);
        }
    }

    AXUDIO_API bool AxUdio_OpenStream(void* instance, const unsigned char* fileBytes, size_t size) {
        if (!instance || !fileBytes || size == 0) return false;
        auto ctx = static_cast<AxUdioContext*>(instance);

        ctx->data.assign(fileBytes, fileBytes + size);
        ctx->position = 0;
        ctx->running = true;
        ctx->analysis.isPlaying = 1;
        return true;
    }

    AXUDIO_API bool AxUdio_ReadNextChunk(void* instance) {
        if (!instance) return false;
        auto ctx = static_cast<AxUdioContext*>(instance);

        if (!ctx->running || ctx->position >= ctx->data.size()) {
            ctx->analysis.isPlaying = 0;
            ctx->running = false;
            return false;
        }

        // Продвигаем позицию чтения (чанк по 4096 байт)
        ctx->position += 4096;
        if (ctx->position > ctx->data.size()) {
            ctx->position = ctx->data.size();
        }

        // Генерируем тестовые значения громкости для визуализатора (позже заменишь на вызовы из AxUdioDek/AxUdioBitrate)
        float mockVolume = (float)(rand() % 80) / 100.0f + 0.1f;
        ctx->analysis.rmsVolume = mockVolume;
        ctx->analysis.peakVolume = std::min(1.0f, mockVolume + 0.15f);

        return true;
    }

    AXUDIO_API AnalysisData AxUdio_GetAnalysis(void* instance) {
        if (!instance) return { 0, 0.0f, 0.0f, 0 };
        return static_cast<AxUdioContext*>(instance)->analysis;
    }
}