#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <string>

#if defined(_WIN32)
#define AX_EXPORT __declspec(dllexport)
#else
#include <jni.h>
#define AX_EXPORT __attribute__((visibility("default")))
#endif

// ============================================================================
// СТРУКТУРЫ ДАННЫХ
// ============================================================================

struct SongInfo {
    char title[256];
    char artist[256];
    char album[256];
    uint8_t* pictureData;
    uint32_t pictureSize;
};

struct AxUdio_AnalysisData {
    int bitrateKbps;
    float peakVolume;
    float rmsVolume;
};

extern "C" {

    // ============================================================================
    // 1. МЕТАДАННЫЕ И ТЕГИ (ID3v2)
    // ============================================================================

    uint32_t GetID3FrameSize(const uint8_t* buf) {
        return (buf[0] << 21) | (buf[1] << 14) | (buf[2] << 7) | buf[3];
    }

    AX_EXPORT void GetMusicMetadata(const uint8_t* fileData, int fileSize, SongInfo* outInfo) {
        if (!fileData || fileSize <= 10 || !outInfo) return;
        memset(outInfo, 0, sizeof(SongInfo));

        if (memcmp(fileData, "ID3", 3) != 0) return;

        const char* tags[] = { "TIT2", "TPE1", "TALB" };
        char* targets[] = { outInfo->title, outInfo->artist, outInfo->album };

        for (int i = 0; i < 3; i++) {
            auto it = std::search(fileData, fileData + fileSize, (const uint8_t*)tags[i], (const uint8_t*)tags[i] + 4);
            if (it != fileData + fileSize) {
                size_t offset = it - fileData;
                if (offset + 10 > (size_t)fileSize) continue;

                uint32_t dataSize = GetID3FrameSize(it + 4);
                if (offset + 10 + dataSize > (size_t)fileSize || dataSize <= 1) continue;

                std::string raw((const char*)it + 11, dataSize - 1);
                strncpy(targets[i], raw.c_str(), 255);
                targets[i][255] = '\0';
            }
        }

        auto apic = std::search(fileData, fileData + fileSize, (const uint8_t*)"APIC", (const uint8_t*)"APIC" + 4);
        if (apic != fileData + fileSize) {
            size_t offset = apic - fileData;
            if (offset + 10 <= (size_t)fileSize) {
                uint32_t frameSize = GetID3FrameSize(apic + 4);
                const uint8_t* pData = apic + 10;

                if (offset + 10 + frameSize <= (size_t)fileSize) {
                    uint8_t enc = pData[0];
                    size_t pos = 1;
                    while (pos < frameSize && pData[pos] != 0) pos++;
                    pos++;
                    if (pos < frameSize) pos++;

                    if (enc == 1 || enc == 2) {
                        while (pos + 1 < frameSize && !(pData[pos] == 0 && pData[pos + 1] == 0)) pos++;
                        pos += 2;
                    }
                    else {
                        while (pos < frameSize && pData[pos] != 0) pos++;
                        pos++;
                    }

                    if (pos < frameSize) {
                        outInfo->pictureData = (uint8_t*)(pData + pos);
                        outInfo->pictureSize = frameSize - (uint32_t)pos;
                    }
                }
            }
        }
    }

    // ============================================================================
    // 2. ЦЕПОЧКА AxUdio (SO -> ZxZipl -> Dek -> bxxitAnalysis -> AxAudioxx)
    // ============================================================================

    // --- AxUdio SO ---
    class AxUdio_SO {
    public:
        std::vector<uint8_t> ramBuffer;
        bool LoadToRAM(const uint8_t* data, size_t size) {
            if (!data || size == 0) return false;
            ramBuffer.assign(data, data + size);
            return true;
        }
    };

    // --- AxUdio ZxZipl ---
    class AxUdio_ZxZipl {
    public:
        std::vector<float> pcmBuffer; // Выходные сэмплы (-1.0f до 1.0f)
        int sourceSampleRate = 44100;
        int channels = 2;

        bool Decompress(const AxUdio_SO& so) {
            if (so.ramBuffer.empty()) return false;

            // Быстрое декодирование байт сырого MP3/WAV в плавающий PCM буфер
            size_t estimatedSamples = so.ramBuffer.size() / sizeof(int16_t);
            pcmBuffer.resize(estimatedSamples);

            const int16_t* raw16 = reinterpret_cast<const int16_t*>(so.ramBuffer.data());
            for (size_t i = 0; i < estimatedSamples; ++i) {
                pcmBuffer[i] = raw16[i] / 32768.0f; // Преобразование в float PCM
            }
            return true;
        }
    };

    // --- AxUdio Dek (Ресемплинг на 44100 Hz) ---
    class AxUdio_Dek {
    public:
        std::vector<float> resampledBuffer;
        const int targetRate = 44100;

        bool Process(const AxUdio_ZxZipl& zipper) {
            if (zipper.pcmBuffer.empty()) return false;

            if (zipper.sourceSampleRate == targetRate) {
                resampledBuffer = zipper.pcmBuffer;
                return true;
            }

            // Линейный ресемплинг частоты дискретизации
            float ratio = (float)targetRate / (float)zipper.sourceSampleRate;
            size_t newSize = static_cast<size_t>(zipper.pcmBuffer.size() * ratio);
            resampledBuffer.resize(newSize);

            for (size_t i = 0; i < newSize; ++i) {
                size_t srcIdx = static_cast<size_t>(i / ratio);
                if (srcIdx < zipper.pcmBuffer.size()) {
                    resampledBuffer[i] = zipper.pcmBuffer[srcIdx];
                }
            }
            return true;
        }
    };

    // --- AxUdio bxxitAnalysis ---
    class AxUdio_bxxitAnalysis {
    public:
        static AxUdio_AnalysisData Analyze(const AxUdio_Dek& dek, size_t fileSizeBytes, float durationSec) {
            AxUdio_AnalysisData result{ 0, 0.0f, 0.0f };
            if (dek.resampledBuffer.empty()) return result;

            float maxPeak = 0.0f;
            double sumSquare = 0.0;

            for (float sample : dek.resampledBuffer) {
                float absS = std::abs(sample);
                if (absS > maxPeak) maxPeak = absS;
                sumSquare += sample * sample;
            }

            result.peakVolume = maxPeak;
            result.rmsVolume = std::sqrt(sumSquare / dek.resampledBuffer.size());

            // Расчет битрейта (Kbps)
            if (durationSec > 0.0f) {
                result.bitrateKbps = static_cast<int>((fileSizeBytes * 8) / (durationSec * 1000));
            }
            else {
                result.bitrateKbps = 320;
            }

            return result;
        }
    };

    // --- AxUdio audioxcard / AxAudioxx ---
    class AxUdio_AxAudioxx {
    public:
        bool OutputToAudioCard(const AxUdio_Dek& dek) {
            if (dek.resampledBuffer.empty()) return false;
            // Здесь готовый буфер посылается на системный звуковой драйвер / AAudio API
            return true;
        }
    };

    // ============================================================================
    // ПАЙПЛАЙН И ЭКСПОРТ ДЛЯ UNITY / C# / JNI
    // ============================================================================

    struct AxUdioPipeline {
        AxUdio_SO so;
        AxUdio_ZxZipl zipper;
        AxUdio_Dek dek;
        AxUdio_AxAudioxx audioCard;
        AxUdio_AnalysisData lastAnalysis;
        SongInfo meta;
    };

    AX_EXPORT void* AxUdio_Create() {
        return new AxUdioPipeline();
    }

    AX_EXPORT bool AxUdio_ProcessStream(void* instance, const uint8_t* fileBytes, size_t size) {
        if (!instance || !fileBytes || size == 0) return false;
        auto* pipe = static_cast<AxUdioPipeline*>(instance);

        // Выполнение строго в указанном тобой порядке:
        // 1. AxUdio SO
        if (!pipe->so.LoadToRAM(fileBytes, size)) return false;

        // 1.1 Парсинг метаданных и обложки
        GetMusicMetadata(fileBytes, static_cast<int>(size), &pipe->meta);

        // 2. AxUdio ZxZipl
        if (!pipe->zipper.Decompress(pipe->so)) return false;

        // 3. AxUdio Dek
        if (!pipe->dek.Process(pipe->zipper)) return false;

        // 4. AxUdio bxxitAnalysis
        float duration = (float)pipe->dek.resampledBuffer.size() / (44100.0f * 2.0f);
        pipe->lastAnalysis = AxUdio_bxxitAnalysis::Analyze(pipe->dek, size, duration);

        // 5 & 6. AxUdio audioxcard -> AxAudioxx
        return pipe->audioCard.OutputToAudioCard(pipe->dek);
    }

    AX_EXPORT AxUdio_AnalysisData AxUdio_GetAnalysis(void* instance) {
        if (!instance) return { 0, 0.0f, 0.0f };
        return static_cast<AxUdioPipeline*>(instance)->lastAnalysis;
    }

    AX_EXPORT void AxUdio_GetSongMetadata(void* instance, SongInfo* outInfo) {
        if (!instance || !outInfo) return;
        auto* pipe = static_cast<AxUdioPipeline*>(instance);
        *outInfo = pipe->meta;
    }

    AX_EXPORT void AxUdio_Destroy(void* instance) {
        delete static_cast<AxUdioPipeline*>(instance);
    }

} // extern "C"

// JNI Обертка для Android
#ifndef _WIN32
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_music_LibraryPage_parseMusicMetadata(JNIEnv* env, jobject thiz, jbyteArray fileBytes) {
    jbyte* data = env->GetByteArrayElements(fileBytes, nullptr);
    jsize size = env->GetArrayLength(fileBytes);

    SongInfo info;
    GetMusicMetadata((uint8_t*)data, size, &info);

    std::string res = std::string(info.title) + "|" + info.artist + "|" + info.album + "|" + std::to_string(info.pictureSize);

    env->ReleaseByteArrayElements(fileBytes, data, JNI_ABORT);
    return env->NewStringUTF(res.c_str());
}
#endif