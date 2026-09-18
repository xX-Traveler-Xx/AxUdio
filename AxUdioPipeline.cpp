#include "AxUdioCore.h"

struct AxUdioPipeline {
    AxUdio_SO so;
    AxUdio_ZxZipl zipl;
    AxUdio_Dek dek;
    AxUdio_audioxcard audioCard;
    AxUdio_AnalysisData lastAnalysis;
};

extern "C" {

    AX_EXPORT void* AxUdio_Create() {
        return new AxUdioPipeline();
    }

    AX_EXPORT bool AxUdio_ProcessStream(void* instance, const uint8_t* fileBytes, size_t size) {
        if (!instance || !fileBytes || size == 0) return false;
        auto* pipe = static_cast<AxUdioPipeline*>(instance);

        if (!pipe->so.LoadToRAM(fileBytes, size)) return false;
        if (!pipe->zipl.DecompressStream(pipe->so.ramBuffer)) return false;
        if (!pipe->dek.Process44100(pipe->zipl.pcmBuffer, pipe->zipl.sourceSampleRate)) return false;

        pipe->lastAnalysis = AxUdio_bxxitAnalysis::Analyze(pipe->dek.resampledBuffer, size);

        return pipe->audioCard.SendToAxAudioxx(pipe->dek.resampledBuffer);
    }

    AX_EXPORT AxUdio_AnalysisData AxUdio_GetAnalysis(void* instance) {
        if (!instance) return AxUdio_AnalysisData{ 0, 0.0f, 0.0f };
        return static_cast<AxUdioPipeline*>(instance)->lastAnalysis;
    }

    AX_EXPORT void AxUdio_Destroy(void* instance) {
        delete static_cast<AxUdioPipeline*>(instance);
    }

} // extern "C"