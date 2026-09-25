#include "AxUdioCore.h"

#if defined(_WIN32)
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uuid.lib")

bool AxUdio_ZxZipl::DecompressStream(const std::vector<uint8_t>& rawBuffer) {
    if (rawBuffer.empty()) return false;
    pcmBuffer.clear();

    // 1. Инициализация Media Foundation
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) return false;

    // Создание IStream из сырого буфера в ОЗУ
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

    // 2. Получаем родной формат файла и запрашиваем распаковку в Float PCM
    IMFMediaType* pNativeType = NULL;
    hr = pReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &pNativeType);
    if (FAILED(hr)) {
        pReader->Release();
        MFShutdown();
        return false;
    }

    IMFMediaType* pUncompressedType = NULL;
    MFCreateMediaType(&pUncompressedType);
    pNativeType->CopyAllItems(pUncompressedType);
    pNativeType->Release();

    // Задаем выходу несжатый 32-битный плавающий формат (Float PCM)
    pUncompressedType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);

    hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pUncompressedType);
    pUncompressedType->Release();
    if (FAILED(hr)) {
        pReader->Release();
        MFShutdown();
        return false;
    }

    // 3. Распаковка всех кадров в pcmBuffer
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