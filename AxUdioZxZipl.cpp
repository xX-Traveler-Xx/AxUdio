#include "AxUdioCore.h"

#if defined(_WIN32)
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

// Все необходимые библиотеки Media Foundation для успешной сборки:
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")     // Исправляет неразрешенные символы MF_MT_* и MFAudioFormat_*
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uuid.lib")

bool AxUdio_ZxZipl::DecompressStream(const std::vector<uint8_t>& rawBuffer) {
    if (rawBuffer.empty()) return false;
    pcmBuffer.clear();

    // 1. Инициализация Media Foundation
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) return false;

    // Создание IStream из сырого байтового буфера в RAM
    IStream* pMemStream = SHCreateMemStream(rawBuffer.data(), static_cast<UINT>(rawBuffer.size()));
    if (!pMemStream) {
        MFShutdown();
        return false;
    }

    IMFByteStream* pByteStream = NULL;
    hr = MFCreateMFByteStreamOnStream(pMemStream, &pByteStream);
    pMemStream->Release();
    if (FAILED(hr)) {
        MFShutdown();
        return false;
    }

    IMFSourceReader* pReader = NULL;
    hr = MFCreateSourceReaderFromByteStream(pByteStream, NULL, &pReader);
    pByteStream->Release();
    if (FAILED(hr)) {
        MFShutdown();
        return false;
    }

    // 2. Конфигурация декодера: PCM Float 32-bit, 44100 Hz, Stereo
    IMFMediaType* pPartialType = NULL;
    hr = MFCreateMediaType(&pPartialType);
    if (FAILED(hr)) {
        pReader->Release();
        MFShutdown();
        return false;
    }

    pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
    pPartialType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
    pPartialType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
    pPartialType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 8);
    pPartialType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 44100 * 8);
    pPartialType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32);

    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pPartialType);
    pPartialType->Release();
    if (FAILED(hr)) {
        pReader->Release();
        MFShutdown();
        return false;
    }

    // 3. Декодирование аудиопотока в float PCM
    while (true) {
        DWORD flags = 0;
        IMFSample* pSample = NULL;

        hr = pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &flags, NULL, &pSample);
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
            if (pSample) pSample->Release();
            break;
        }

        if (pSample) {
            IMFMediaBuffer* pBuffer = NULL;
            pSample->ConvertToContiguousBuffer(&pBuffer);
            if (pBuffer) {
                BYTE* pAudioData = NULL;
                DWORD currentLength = 0;
                pBuffer->Lock(&pAudioData, NULL, &currentLength);

                if (pAudioData && currentLength > 0) {
                    const float* floatData = reinterpret_cast<const float*>(pAudioData);
                    size_t sampleCount = currentLength / sizeof(float);
                    pcmBuffer.insert(pcmBuffer.end(), floatData, floatData + sampleCount);
                }

                pBuffer->Unlock();
                pBuffer->Release();
            }
            pSample->Release();
        }
    }

    pReader->Release();
    MFShutdown();

    return !pcmBuffer.empty();
}
#endif