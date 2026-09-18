#include "AxUdioCore.h"

class AxUdio_Dek {
public:
    std::vector<float> resampledBuffer;
    const int targetRate = 44100;

    bool ResampleTo44100(const std::vector<float>& inputPcm, int inputRate) {
        if (inputPcm.empty()) return false;

        float ratio = (float)targetRate / (float)inputRate;
        size_t newSize = static_cast<size_t>(inputPcm.size() * ratio);
        resampledBuffer.resize(newSize);

        for (size_t i = 0; i < newSize; ++i) {
            size_t srcIdx = static_cast<size_t>(i / ratio);
            if (srcIdx < inputPcm.size()) {
                resampledBuffer[i] = inputPcm[srcIdx];
            }
        }
        return true;
    }
};