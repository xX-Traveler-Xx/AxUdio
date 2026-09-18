#include "AxUdioCore.h"

bool AxUdio_ZxZipl::DecompressStream(const std::vector<uint8_t>& rawBuffer) {
    if (rawBuffer.empty()) return false;

    size_t estimatedSamples = rawBuffer.size() / sizeof(int16_t);
    pcmBuffer.resize(estimatedSamples);

    const int16_t* raw16 = reinterpret_cast<const int16_t*>(rawBuffer.data());
    for (size_t i = 0; i < estimatedSamples; ++i) {
        pcmBuffer[i] = raw16[i] / 32768.0f;
    }
    return true;
}