// AxUdioDek.cpp
#include "AxUdioCore.h"


bool AxUdio_Dek::Process44100(const std::vector<float>& pcm, int currentRate) {
    resampledBuffer.clear();

    if (pcm.empty()) {
        return false;
    }

    // Защита от выхода за границы при копировании/ресемплинге
    resampledBuffer.reserve(pcm.size());
    for (size_t i = 0; i < pcm.size(); ++i) {
        resampledBuffer.push_back(pcm[i]);
    }

    return !resampledBuffer.empty();
}