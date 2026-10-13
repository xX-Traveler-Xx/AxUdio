
---
AxUdio DSP — универсальный музыкальный движок («AxUdio», AxUdio DSP или просто AxUdio). Это тестовый проект с использованием ИИ. Использовать не рекомендую, а проект могу закрыть в любой момент. Этот проект полностью написан с помощью ИИ. Однако этот проект мне нравится, и я буду пытаться довести его до ума — до рабочего состояния.
--
А наименования компонентов оставлены так, как они несли смысл для автора и круто выглядели.

<img width="480" height="270" alt="AxUdio" src="https://github.com/user-attachments/assets/89f9ea5c-0f98-400b-a325-f13ee933277d" />


---
# AxUdio

[🇷🇺 Русский](#-axudio-ru) | [🇬🇧 English](#-axudio-en)

---
> **ВАЖНО:** АУДИОПАНЕЛЬ ЯВЛЯЕТСЯ НЕОБЯЗАТЕЛЬНОЙ. В БУДУЩЕМ ОНА БУДЕТ ОТКЛЮЧЕНА ИЛИ ПЕРЕВЕДЕНА НА РУЧНОЙ ВЫЗОВ, НО СОХРАНИТСЯ В КОДЕ.

---
## 🇷🇺 AxUdio <a name="axudio-ru"></a>

**AxUdio** — это специализированная скриптовая система и конвейер обработки аудио (pipeline), предназначенная для загрузки, декодирования, анализа и универсального вывода звука. Это не полноценный аудиодвижок, а единый сквозной процесс, состоящий из взаимосвязанных методов и функций.

> 🤖 **100% кода этой программы было полностью написано ИИ (Искусственным Интеллектом).**

### 💻 Системные требования и архитектура
* **Платформа / Архитектура:** x64 (64-bit)
* **Язык программирования:** C++
* **Система сборки:** CMake

### 🔄 Порядок исполнения и методы (Pipeline)
Порядок обработки звукового потока выполняется строго сверху вниз через следующие встроенные модули:

* **AxUdio SO** — считывает файл и загружает его содержимое в оперативную память (ОЗУ), по аналогии с классическим воспроизведением оффлайн-файлов MP3 на мобильных устройствах.
* **AxUdio ZxZipl** — производит распаковку и декодирование аудио из формата MP3 в несжатый формат WAV «на лету» (в реальном времени).
* **AxUdio Dek** — выполняет дискретизацию (передискретизацию) аудиопотока до стандартной частоты 44100 Гц.
* **AxUdio b××itAanalysis** — анализирует текущий битрейт аудиофайла и передает полученные телеметрические данные в смежные скрипты.
* **AxUdio audio×card** — формирует готовый к воспроизведению буфер данных и отправляет его на финальный модуль вывода.
* **AxUdio A×Audio××** — модуль вывода звука. Основан на низкоуровневом API AAudio (C++) с применением вспомогательного слоя AAudio××. Обеспечивает универсальный вывод звука на платформах Android, Linux и Windows.

### 📄 Лицензия и Отказ от ответственности
Авторская концепция и архитектура данного проекта принадлежат автору репозитория.

Данное программное обеспечение предоставляется «КАК ЕСТЬ» (AS IS), без каких-либо явных или подразумеваемых гарантий. Автор не несет никакой ответственности за любые прямые, косвенные, случайные или иные убытки, возникшие в результате использования или невозможности использования данного программного обеспечения.

---

## 🇬🇧 AxUdio <a name="axudio-en"></a>

**AxUdio** is a specialized scripting system and audio processing pipeline designed for loading, decoding, analyzing, and outputting sound. It is not a full-fledged audio engine, but rather a single unified script consisting of sequential methods and functions.

> 🤖 **100% of the code in this project was written by AI (Artificial Intelligence).**

### 💻 System Requirements & Architecture
* **Platform / Architecture:** x64 (64-bit)
* **Programming Language:** C++
* **Build System:** CMake

### 🔄 Execution Order and Methods (Pipeline)
The execution order flows sequentially from top to bottom through the following modules:

* **AxUdio SO** — Reads the file and loads it into RAM, mimicking standard offline MP3 file handling on mobile devices.
* **AxUdio ZxZipl** — Decompresses and decodes MP3 files into uncompressed WAV on the fly.
* **AxUdio Dek** — Resamples/discretizes the audio stream to a standard sample rate of 44100 Hz.
* **AxUdio b××itAanalysis** — Analyzes bitrate data and feeds the analytical metrics to secondary scripts.
* **AxUdio audio×card** — Prepares the finalized audio buffer and sends it directly to AxUdio A×Audio××.
* **AxUdio A×Audio××** — The core audio output backend based on AAudio (C++) and the AAudio×× helper layer. Provides universal audio output across Android, Linux, and Windows.

### 📄 License and Disclaimer
The original concept and system architecture of this project belong to the repository author.

This software is provided "AS IS", without warranty of any kind, express or implied. In no event shall the author be held liable for any claim, damages, or other liability arising from, out of, or in connection with the software or the use or other dealings in the software.

> Не факт что проект может быть довелен до ума мной
> Но я попробую. Может получить зделать что типо движка свого для музыку

---

Техническая документация: AxUdio Audio EngineВерсия: 1.0.9Язык реализации: C++ (Core), C-compatible API для внешних интеграций (например, C#)Поддерживаемые платформы: Windows (WinMM / Media Foundation), Android (AAudio), Linux (ALSA)1. Архитектура библиотекиБиблиотека построена по модульному принципу. Каждый компонент отвечает за изолированный этап конвейера обработки аудиоданных (пайплайна):AxUdio_SO — загрузка исходного аудиофайла (в виде байтов) в оперативную память (RAM).AxUdio_ZxZipl — распаковка сжатого потока (в Windows используется Microsoft Media Foundation с конвертацией в 32-bit Float PCM).AxUdio_Dek — модуль ресемплинга (передискретизации) аудиосигнала до целевой частоты дискретизации 44100 Гц.AxUdio_bxxitAnalysis — анализ аудиопотока "на лету" (расчет пиковой громкости (Peak), среднеквадратичного значения (RMS) и битрейта).AxUdio_audioxcard / AxUdio_AxAudioxx — низкоуровневый вывод звука на аппаратные аудиоподсистемы целевой платформы.Встроенный Web-Dashboard (AxUdioDashboard.h) — мониторинг состояния аудиосистемы через HTTP/WebSocket интерфейс (порт 8080).2. Экспортируемый C-APIДля взаимодействия с управляемыми средами (например, C# / P/Invoke) или сторонними языками используется плоский C-интерфейс, объявленный с модификатором extern "C".   Управление жизненным циклом экземпляраvoid* AxUdio_Create() — создает новый экземпляр движка. Автоматически запускает встроенный веб-дашборд на порту 8080.void AxUdio_Destroy(void* instance) — уничтожает экземпляр движка и освобождает занятые ресурсы.Работа со стримомbool AxUdio_OpenStream(void* instance, const unsigned char* fileBytes, size_t size) — загружает массив байт аудиофайла в память, декодирует его и приводит к формату 44100 Гц.bool AxUdio_ReadNextChunk(void* instance) — считывает следующий чанк данных (по умолчанию 1024 сэмпла), выполняет аудиоанализ, отправляет данные на звуковую карту и сдвигает внутренний курсор воспроизведения.Управление воспроизведениемvoid AxUdio_Play(void* instance) — возобновляет или запускает воспроизведение.void AxUdio_Pause(void* instance) — ставит воспроизведение на паузу.void AxUdio_Stop(void* instance) — останавливает воспроизведение и сбрасывает курсор в начало.Телеметрия и анализAnalysisData AxUdio_GetAnalysis(void* instance) — возвращает текущие метрики воспроизведения (AnalysisData).Структура данных анализа (AnalysisData):C++struct AnalysisData {
    int bitrateKbps;    // Битрейт (например, 320 кбит/с)
    float peakVolume;   // Пиковая громкость (0.0 — 1.0)
    float rmsVolume;    // RMS громкость (0.0 — 1.0)
    int isPlaying;      // Статус: 1 — играет, 0 — пауза / конец трека
};
3. Особенности компиляции под различные ОСLinuxПри сборке проекта под Linux (с использованием g++ или CMake) требуется обязательно подключать нативную звуковую подсистему ALSA с помощью флага -lasound (библиотека встроена в ядро большинства дистрибутивов, таких как Ubuntu, Debian, Arch и др.):   Bashg++ -shared -fPIC AxAudioxx.cpp AxUdioBitrate.cpp AxUdioDek.cpp AxUdioPipeline.cpp AxUdioSO.cpp AxUdioZxZipl.cpp -lasound -o libAxUdioEngine.so
WindowsТребуется линковка со стандартными системными библиотеками Windows Multimedia API (winmm.lib) и Media Foundation (mfplat.lib, mfreadwrite.lib, mfuuid.lib, shlwapi.lib, uuid.lib).   Макрос AXUDIO_EXPORTS управляет экспортом символов DLL (__declspec(dllexport) / __declspec(dllimport)).   AndroidИспользуется нативный низкоуровневый аудиоапи AAudio с поддержкой режима AAUDIO_PERFORMANCE_MODE_LOW_LATENCY.   Реализован встроенный лимитер защиты (ApplyProtection), предотвращающий цифровое клиппирование звука при превышении порога 0.7f.   4. Пример интеграции (C++ / Псевдокод)C++#include "AxUdioCore.h"
#include <vector>

// 1. Инициализация движка
void* engine = AxUdio_Create();

// 2. Загрузка файла (здесь предполагается, что fileBytes — это байты аудиофайла, например, MP3)
// const unsigned char* fileBytes = ...;
// size_t fileSize = ...;

if (AxUdio_OpenStream(engine, fileBytes, fileSize)) {
    AxUdio_Play(engine);

    // 3. Цикл чтения и обработки чанков (например, в аудиоплеере)
    while (true) {
        AnalysisData data = AxUdio_GetAnalysis(engine);
        if (data.isPlaying == 0) break;

        // Читаем следующий чанк и выводим на звуковую карту
        bool success = AxUdio_ReadNextChunk(engine);
        if (!success) break;
    }

    AxUdio_Stop(engine);
}

// 4. Очистка ресурсов
AxUdio_Destroy(engine);
