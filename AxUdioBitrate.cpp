#include "AxUdioCore.h"

AnalysisData AxUdio_bxxitAnalysis::Analyze(const std::vector<float>& pcm, size_t fileSize) {
    AnalysisData result{ 320, 0.0f, 0.0f, 1 };
    if (pcm.empty()) {
        result.isPlaying = 0;
        return result;
    }

    float maxPeak = 0.0f;
    double sumSq = 0.0;

    for (float s : pcm) {
        float absS = std::abs(s);
        if (absS > maxPeak) maxPeak = absS;
        sumSq += s * s;
    }

    result.peakVolume = maxPeak;
    result.rmsVolume = static_cast<float>(std::sqrt(sumSq / pcm.size()));
    result.isPlaying = 1;
    return result;
}