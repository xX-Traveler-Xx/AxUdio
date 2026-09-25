#include "AxUdioCore.h"

// Объявляем функцию запуска веб-сервера из AxUdioHttpServer.cpp
void StartAxUdioDashboard(int port = 8080);

struct AxUdioContext {
    AxUdio_SO so;
    AxUdio_ZxZipl zipl;
    AxUdio_Dek dek;
    AxUdio_bxxitAnalysis bitrate;
    AxUdio_audioxcard audioCard;

    std::vector<float> finalPcm;
    size_t cursor = 0;
    AnalysisData currentAnalysis = { 320, 0.0f, 0.0f, 0 };
};

extern "C" {

    // 1. Создание контекста движка и запуск HTTP-сервера с дашбордом
    AXUDIO_API void* AxUdio_Create() {
        // Автоматически запускает HTTP-сервер и открывает браузер при инициализации
        StartAxUdioDashboard(8080);
        return new (std::nothrow) AxUdioContext();
    }

    // 2. Уничтожение контекста и очистка памяти
    AXUDIO_API void AxUdio_Destroy(void* instance) {
        if (instance) {
            delete static_cast<AxUdioContext*>(instance);
        }
    }

    // 3. Получение текущих метрик анализа
    AXUDIO_API AnalysisData AxUdio_GetAnalysis(void* instance) {
        if (!instance) {
            return AnalysisData{ 0, 0.0f, 0.0f, 0 };
        }
        auto ctx = static_cast<AxUdioContext*>(instance);
        return ctx->currentAnalysis;
    }

    // 4. Открытие аудиопотока
    AXUDIO_API bool AxUdio_OpenStream(void* instance, const unsigned char* fileBytes, size_t size) {
        if (!instance || !fileBytes || size == 0) return false;
        auto ctx = static_cast<AxUdioContext*>(instance);

        // 1. RAM Загрузка
        if (!ctx->so.LoadToRAM(fileBytes, size)) return false;

        // 2. Распаковка
        if (!ctx->zipl.DecompressStream(ctx->so.ramBuffer)) return false;

        // 3. Дискретизация 44100 Hz
        if (!ctx->dek.Process44100(ctx->zipl.pcmBuffer, 44100)) return false;

        ctx->finalPcm = ctx->dek.resampledBuffer;
        ctx->cursor = 0;

        // Если вектор после обработки оказался пустым — возвращаем false
        if (ctx->finalPcm.empty()) {
            ctx->currentAnalysis.isPlaying = 0;
            return false;
        }

        ctx->currentAnalysis.isPlaying = 1;
        return true;
    }

    AXUDIO_API bool AxUdio_ReadNextChunk(void* instance) {
        if (!instance) return false;
        auto ctx = static_cast<AxUdioContext*>(instance);

        if (ctx->finalPcm.empty() || ctx->cursor >= ctx->finalPcm.size()) {
            ctx->currentAnalysis.isPlaying = 0;
            return false;
        }

        size_t chunkSize = 1024;
        size_t remaining = ctx->finalPcm.size() - ctx->cursor;
        size_t count = (std::min)(chunkSize, remaining);

        // Выделяем память и копируем данные по индексам
        std::vector<float> chunk;
        chunk.reserve(count);

        for (size_t i = 0; i < count; ++i) {
            chunk.push_back(ctx->finalPcm[ctx->cursor + i]);
        }

        // Выполнение анализа
        ctx->currentAnalysis = AxUdio_bxxitAnalysis::Analyze(chunk, ctx->finalPcm.size());

        // Вывод на аудиокарту
        ctx->audioCard.SendToAxAudioxx(chunk);

        ctx->cursor += count;
        return true;
    }
    AXUDIO_API void AxUdio_Play(void* instance) {
        if (!instance) return;
        auto ctx = static_cast<AxUdioContext*>(instance);
        ctx->audioCard.driver.Play(); // Или ваша логика возобновления
        ctx->currentAnalysis.isPlaying = 1;
    }

    AXUDIO_API void AxUdio_Pause(void* instance) {
        if (!instance) return;
        auto ctx = static_cast<AxUdioContext*>(instance);
        ctx->audioCard.driver.Pause(); // Или ваша логика паузы
        ctx->currentAnalysis.isPlaying = 0;
    }

    AXUDIO_API void AxUdio_Stop(void* instance) {
        if (!instance) return;
        auto ctx = static_cast<AxUdioContext*>(instance);
        ctx->audioCard.driver.Stop(); // Или ваша логика остановки
        ctx->cursor = 0;
        ctx->currentAnalysis.isPlaying = 0;
    }
}