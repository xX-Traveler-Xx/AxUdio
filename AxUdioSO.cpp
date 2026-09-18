#include "AxUdioCore.h"

class AxUdio_SO {
public:
    std::vector<uint8_t> ramBuffer;

    bool LoadToRAM(const uint8_t* fileData, size_t size) {
        if (!fileData || size == 0) return false;
        ramBuffer.assign(fileData, fileData + size);
        return true;
    }
};