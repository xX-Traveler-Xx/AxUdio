#include "AxUdioCore.h"

bool AxUdio_Dek::Process44100(const std::vector<float>& pcm, int currentRate) {
    if (pcm.empty()) return false;

    float ratio = (float)targetRate / (float)currentRate;
    size_t newSize = static_cast<size_t>(pcm.size() * ratio);
    resampledBuffer.resize(newSize);

    for (size_t i = 0; i < newSize; ++i) {
        size_t srcIdx = static_cast<size_t>(i / ratio);
        if (srcIdx < pcm.size()) {
            resampledBuffer[i] = pcm[srcIdx];
        }
    }
    return true;
}