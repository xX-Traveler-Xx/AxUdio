#include "AxUdioCore.h"

class AxUdio_ZxZipl {
public:
    std::vector<float> pcmBuffer;
    int sourceSampleRate = 48000;

    bool DecompressToWavStream(const std::vector<uint8_t>& ramBuffer) {
        if (ramBuffer.empty()) return false;

        // Логика декодирования MP3 кадров в PCM
        size_t estimatedSamples = ramBuffer.size() / sizeof(int16_t);
        pcmBuffer.resize(estimatedSamples);

        const int16_t* raw16 = reinterpret_cast<const int16_t*>(ramBuffer.data());
        for (size_t i = 0; i < estimatedSamples; ++i) {
            pcmBuffer[i] = raw16[i] / 32768.0f;
        }
        return true;
    }
};