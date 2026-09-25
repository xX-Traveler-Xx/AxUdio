#include "AxUdioCore.h"

bool AxUdio_SO::LoadToRAM(const uint8_t* fileData, size_t size) {
    if (!fileData || size == 0) return false;
    ramBuffer.assign(fileData, fileData + size);
    return true;
}