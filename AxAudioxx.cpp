#include "AxUdioCore.h"

bool AxUdio_AxAudioxx::OutputToHardware(const std::vector<float>& buffer) {
    if (buffer.empty()) return false;
    return true;
}

bool AxUdio_audioxcard::SendToAxAudioxx(const std::vector<float>& buffer) {
    return driver.OutputToHardware(buffer);
}