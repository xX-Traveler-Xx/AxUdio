#include "AxUdioCore.h"

class AxUdio_bxxitAnalysis {
public:
    static AxUdio_AnalysisData Analyze(const std::vector<float>& pcm, size_t fileSize) {
        AxUdio_AnalysisData result{ 320, 0.0f, 0.0f };
        if (pcm.empty()) return result;

        float maxPeak = 0.0f;
        double sumSq = 0.0;

        for (float s : pcm) {
            float absS = std::abs(s);
            if (absS > maxPeak) maxPeak = absS;
            sumSq += s * s;
        }

        result.peakVolume = maxPeak;
        result.rmsVolume = std::sqrt(sumSq / pcm.size());
        return result;
    }
};