#include "AxUdioCore.h"

// Подключаем объявления наших классов из соседних файлов
#include "AxUdioSO.cpp"
#include "AxUdioZxZipl.cpp"
#include "AxUdioDek.cpp"
#include "AxUdioBitrate.cpp"
#include "AxAudioxx.cpp"

struct AxUdioPipeline {
    AxUdio_SO so;
    AxUdio_ZxZipl zipper;
    AxUdio_Dek dek;
    AxUdio_AxAudioxx audioCard;
    AxUdio_AnalysisData lastAnalysis;
};

extern "C" {

    AX_EXPORT void* AxUdio_Create() {
        return new AxUdioPipeline();
    }

    AX_EXPORT bool AxUdio_ProcessStream(void* instance, const uint8_t* fileBytes, size_t size) {
        if (!instance || !fileBytes || size == 0) return false;
        auto* pipe = static_cast<AxUdioPipeline*>(instance);

        // ПОРЯДОК ИСПОЛНЕНИЯ:
        // 1. AxUdio SO
        if (!pipe->so.LoadToRAM(fileBytes, size)) return false;

        // 2. AxUdio ZxZipl
        if (!pipe->zipper.DecompressToWavStream(pipe->so.ramBuffer)) return false;

        // 3. AxUdio Dek
        if (!pipe->dek.ResampleTo44100(pipe->zipper.pcmBuffer, pipe->zipper.sourceSampleRate)) return false;

        // 4. AxUdio bxxitAnalysis
        pipe->lastAnalysis = AxUdio_bxxitAnalysis::Analyze(pipe->dek.resampledBuffer, size);

        // 5 & 6. AxUdio audioxcard -> AxUdio AxAudioxx
        return pipe->audioCard.OutputToAAudioHardware(pipe->dek.resampledBuffer);
    }

    AX_EXPORT AxUdio_AnalysisData AxUdio_GetAnalysis(void* instance) {
        if (!instance) return { 0, 0.0f, 0.0f };
        return static_cast<AxUdioPipeline*>(instance)->lastAnalysis;
    }

    AX_EXPORT void AxUdio_Destroy(void* instance) {
        delete static_cast<AxUdioPipeline*>(instance);
    }

} // extern "C"