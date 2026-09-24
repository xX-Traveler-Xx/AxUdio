---
---
# README.md

---

# 🇷🇺 AxUdio

**AxUdio** — это специализированная скриптовая система и конвейер обработки аудио (pipeline), предназначенная для загрузки, декодирования, анализа и универсального вывода звука. Это не полноценный аудиодвижок, а единый сквозной процесс, состоящий из взаимосвязанных методов и функций.

> 🤖 **100% кода этой программы и архитектуры было полностью написано ИИ (Искусственным Интеллектом).**

---

### 💻 Системные требования и Архитектура

* **Платформа / Архитектура:** x64 (64-bit)
* **Язык программирования:** C++
* **Система сборки:** CMake

---

### 🔄 Порядок исполнения и методы (Pipeline)

Порядок обработки звукового потока выполняется строго сверху вниз через следующие встроенные модули:

• **AxUdio SO** — считывает файл и загружает его содержимое в оперативную память (ОЗУ), по аналогии с классическим воспроизведением оффлайн-файлов MP3 на мобильных устройствах.

• **AxUdio ZxZipl** — производит распаковку и декодирование аудио из формата MP3 в несжатый формат WAV «на лету» (в реальном времени).

• **AxUdio Dek** — выполняет дискретизацию (передискретизацию) аудиопотока до стандартной частоты 44100 Гц.

• **AxUdio b××itAanalysis** — анализирует текущий битрейт аудиофайла и передает полученные телеметрические данные в смежные скрипты.

• **AxUdio audio×card** — формирует готовый к воспроизведению буфер данных и отправляет его на финальный модуль вывода.

• **AxUdio A×Audio××** — модуль вывода звука. Основан на низкоуровневом API **AAudio (C++)** с применением вспомогательного слоя **AAudio××**. Обеспечивает универсальный вывод звука на платформах **Android**, **Linux** и **Windows**.

---

---

# 🇬🇧 AxUdio

**AxUdio** is a specialized scripting system and audio processing pipeline designed for loading, decoding, analyzing, and outputting sound. It is not a full-fledged audio engine, but rather a single unified script consisting of sequential methods and functions.

> 🤖 **100% of the code and architecture in this project was written by AI (Artificial Intelligence).**

---

### 💻 System Requirements & Architecture

* **Platform / Architecture:** x64 (64-bit)
* **Programming Language:** C++
* **Build System:** CMake

---

### 🔄 Execution Order and Methods (Pipeline)

The execution order flows sequentially from top to bottom through the following modules:

• **AxUdio SO** — Reads the file and loads it into RAM, mimicking standard offline MP3 file handling on mobile devices.

• **AxUdio ZxZipl** — Decompresses and decodes MP3 files into uncompressed WAV on the fly.

• **AxUdio Dek** — Resamples/discretizes the audio stream to a standard sample rate of 44100 Hz.

• **AxUdio b××itAanalysis** — Analyzes bitrate data and feeds the analytical metrics to secondary scripts.

• **AxUdio audio×card** — Prepares the finalized audio buffer and sends it directly to **AxUdio A×Audio××**.

• **AxUdio A×Audio××** — The core audio output backend based on **AAudio (C++)** and the **AAudio××** helper layer. Provides universal audio output across **Android**, **Linux**, and **Windows**.

---
---
