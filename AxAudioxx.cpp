#include "AxUdioCore.h"

class AxUdio_AxAudioxx {
public:
    bool OutputToAAudioHardware(const std::vector<float>& finalBuffer) {
        if (finalBuffer.empty()) return false;
        // Здесь выполняется вызов API AAudio (AAudioStream_write)
        return true;
    }
};