---
AxUdio DSP — универсальный музыкальный движок («AxUdio», AxUdio DSP или просто AxUdio). Это тестовый проект с использованием ИИ. Использовать не рекомендую, а проект могу закрыть в любой момент. Этот проект полностью написан с помощью ИИ. Однако этот проект мне нравится, и я буду пытаться довести его до ума — до рабочего состояния.
--
А наименования компонентов оставлены так, как они несли смысл для автора и круто выглядели.

<img width="480" height="270" alt="AxUdio" src="https://github.com/user-attachments/assets/89f9ea5c-0f98-400b-a325-f13ee933277d" />


---
# AxUdio

[🇷🇺 Русский](#-axudio-ru) | [🇬🇧 English](#-axudio-en)

---
''' Панель лог и управления'''
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AxUdio Audio Engine Control Panel</title>
    <style>
        :root {
            --bg-color: #0f111a;
            --card-bg: #1a1d2d;
            --accent-green: #00ff88;
            --accent-yellow: #ffcc00;
            --accent-red: #ff3366;
            --text-color: #e0e6ed;
            --text-dim: #7a889b;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            font-family: system-ui, -apple-system, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
        }

        .container {
            width: 100%;
            max-width: 900px;
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
        }

        .header {
            grid-column: 1 / -1;
            background: var(--card-bg);
            padding: 20px;
            border-radius: 8px;
            border-left: 5px solid var(--accent-green);
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        h1 {
            margin: 0;
            font-size: 20px;
            letter-spacing: 1px;
            color: #fff;
        }

        .card {
            background: var(--card-bg);
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 10px rgba(0,0,0,0.3);
        }

        .full-width {
            grid-column: 1 / -1;
        }

        h2 {
            margin-top: 0;
            font-size: 14px;
            text-transform: uppercase;
            color: var(--text-dim);
            border-bottom: 1px solid #2a2f45;
            padding-bottom: 8px;
        }

        /* Визуализация индикаторов уровня */
        .meter-container {
            margin-bottom: 15px;
        }

        .meter-label {
            display: flex;
            justify-content: space-between;
            font-size: 12px;
            margin-bottom: 5px;
        }

        .meter-bar {
            height: 18px;
            background: #0d0e17;
            border-radius: 4px;
            overflow: hidden;
            position: relative;
        }

        .meter-fill {
            height: 100%;
            width: 0%;
            transition: width 0.05s ease-out;
        }

        .fill-rms {
            background: linear-gradient(90deg, #00bb66, var(--accent-green));
        }

        .fill-peak {
            background: linear-gradient(90deg, #cc9900, var(--accent-yellow));
        }

        /* Контролы */
        .control-group {
            margin-bottom: 15px;
        }

        label {
            display: block;
            font-size: 13px;
            margin-bottom: 5px;
        }

        input[type="range"] {
            width: 100%;
            accent-color: var(--accent-green);
        }

        button {
            background: #2a2f45;
            color: #fff;
            border: 1px solid #3a415e;
            padding: 8px 16px;
            border-radius: 4px;
            cursor: pointer;
            font-weight: bold;
            transition: 0.2s;
        }

        button:hover {
            background: var(--accent-green);
            color: #000;
        }

        /* Лог ошибок и консоль PCM */
        .console-log {
            background: #08090f;
            border: 1px solid #2a2f45;
            border-radius: 4px;
            padding: 10px;
            height: 120px;
            overflow-y: auto;
            font-family: monospace;
            font-size: 11px;
            color: var(--accent-green);
        }

        .log-entry {
            margin-bottom: 3px;
        }

        .log-error {
            color: var(--accent-red);
        }

        .log-warn {
            color: var(--accent-yellow);
        }

        /* Сетка статуса */
        .status-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            font-size: 12px;
        }

        .status-item {
            background: #111320;
            padding: 10px;
            border-radius: 4px;
            text-align: center;
        }

        .status-value {
            font-size: 16px;
            font-weight: bold;
            color: #fff;
            margin-top: 5px;
        }
    </style>
</head>
<body>

    <div class="container">

        <!-- Хедер -->
        <div class="header">
            <div>
                <h1>AXUDIO ENGINE MONITOR</h1>
                <small style="color: var(--text-dim)">Кроссплатформенный аудиопроцессор</small>
            </div>
            <div>
                <span id="conn-status" style="color: var(--accent-red); font-size: 12px; font-weight: bold;">● OFFLINE</span>
            </div>
        </div>

        <!-- Монитор Уровней (RMS / PEAK) -->
        <div class="card">
            <h2>Аудио Измерители</h2>

            <div class="meter-container">
                <div class="meter-label">
                    <span>RMS (Средняя громкость)</span>
                    <span id="val-rms">0.00</span>
                </div>
                <div class="meter-bar">
                    <div id="bar-rms" class="meter-fill fill-rms"></div>
                </div>
            </div>

            <div class="meter-container">
                <div class="meter-label">
                    <span>PEAK (Пиковая громкость)</span>
                    <span id="val-peak">0.00</span>
                </div>
                <div class="meter-bar">
                    <div id="bar-peak" class="meter-fill fill-peak"></div>
                </div>
            </div>
        </div>

        <!-- Управление движком -->
        <div class="card">
            <h2>Настройки Потока</h2>

            <div class="control-group">
                <label for="volume-slider">Громкость Выхода: <span id="val-vol">100%</span></label>
                <input type="range" id="volume-slider" min="0" max="100" value="100" oninput="updateVolume(this.value)">
            </div>

            <div class="control-group" style="display: flex; gap: 10px;">
                <button onclick="sendCommand('pause')">Пауза / Плей</button>
                <button onclick="sendCommand('flush')">Сбросить Буфер</button>
            </div>
        </div>

        <!-- Метрики PCM и Железа -->
        <div class="card full-width">
            <h2>Статус PCM Буфера & Железа</h2>
            <div class="status-grid">
                <div class="status-item">
                    <div>Частота дискретизации</div>
                    <div class="status-value" id="val-rate">44100 Гц</div>
                </div>
                <div class="status-item">
                    <div>Буфер (RingBuffer)</div>
                    <div class="status-value" id="val-buffer">0 сэмплов</div>
                </div>
                <div class="status-item">
                    <div>Драйвер Вывода</div>
                    <div class="status-value" id="val-driver">Инициализация...</div>
                </div>
            </div>
        </div>

        <!-- Консоль логов и ошибок -->
        <div class="card full-width">
            <h2>Журнал Событий и Ошибок Engine</h2>
            <div class="console-log" id="console-log">
                <div class="log-entry">[System] Дашборд готов к подключению...</div>
            </div>
        </div>

    </div>

    <script>
        const WS_URL = "ws://" + window.location.hostname + ":" + (window.location.port || "8080") + "/axudio";
        let ws = null;
        let useHttpFallback = false;
        let pollTimer = null;

        function updateUI(data) {
            document.getElementById('conn-status').innerText = '● ONLINE';
            document.getElementById('conn-status').style.color = 'var(--accent-green)';

            if (data.rms !== undefined) {
                document.getElementById('bar-rms').style.width = Math.min(100, data.rms * 100) + '%';
                document.getElementById('val-rms').innerText = Number(data.rms).toFixed(2);
            }
            if (data.peak !== undefined) {
                document.getElementById('bar-peak').style.width = Math.min(100, data.peak * 100) + '%';
                document.getElementById('val-peak').innerText = Number(data.peak).toFixed(2);

                if (data.peak >= 0.98) {
                    document.getElementById('bar-peak').style.background = 'var(--accent-red)';
                } else {
                    document.getElementById('bar-peak').style.background = 'linear-gradient(90deg, #cc9900, var(--accent-yellow))';
                }
            }

            if (data.bufferSize !== undefined) document.getElementById('val-buffer').innerText = data.bufferSize + ' samples';
            if (data.driver !== undefined) document.getElementById('val-driver').innerText = data.driver;
            if (data.sampleRate !== undefined) document.getElementById('val-rate').innerText = data.sampleRate + ' Гц';

            if (data.error) {
                addLog('[ERROR] ' + data.error, 'error');
            }
        }

        function startHttpPolling() {
            if (pollTimer) return;
            useHttpFallback = true;
            addLog('[HTTP] Переключение на HTTP REST API (опрос каждые 50мс)...', 'warn');

            pollTimer = setInterval(() => {
                fetch('/api/stats')
                    .then(res => res.json())
                    .then(data => updateUI(data))
                    .catch(() => {
                        document.getElementById('conn-status').innerText = '● OFFLINE';
                        document.getElementById('conn-status').style.color = 'var(--accent-red)';
                    });
            }, 50);
        }

        function connect() {
            try {
                ws = new WebSocket(WS_URL);

                ws.onopen = () => {
                    addLog('[WS] Соединение с C++ движком установлено.');
                    if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
                    useHttpFallback = false;
                };

                ws.onmessage = (event) => {
                    try {
                        const data = JSON.parse(event.data);
                        updateUI(data);
                    } catch (e) {
                        addLog('[WS] Ошибка разбора JSON', 'error');
                    }
                };

                ws.onclose = () => {
                    if (!useHttpFallback) {
                        startHttpPolling();
                    }
                };

                ws.onerror = () => {
                    if (ws) ws.close();
                };
            } catch (e) {
                startHttpPolling();
            }
        }

        function updateVolume(val) {
            document.getElementById('val-vol').innerText = val + '%';
            sendCommand('set_volume', { value: val / 100.0 });
        }

        function sendCommand(cmd, payload = {}) {
            const msg = { command: cmd, ...payload };
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(msg));
                addLog('[CMD WS] Отправлена команда: ' + cmd);
            } else {
                fetch('/api/command', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(msg)
                })
                .then(() => addLog('[CMD HTTP] Отправлена команда: ' + cmd))
                .catch(() => addLog('[ERROR] Ошибка отправки команды на C++ бэкенд', 'error'));
            }
        }

        function addLog(msg, type = 'info') {
            const consoleEl = document.getElementById('console-log');
            const entry = document.createElement('div');
            entry.className = 'log-entry' + (type === 'error' ? ' log-error' : (type === 'warn' ? ' log-warn' : ''));

            const time = new Date().toLocaleTimeString();
            entry.innerText = `[${time}] ${msg}`;

            consoleEl.appendChild(entry);
            consoleEl.scrollTop = consoleEl.scrollHeight;
        }

        // Автостарт
        connect();
    </script>

</body>
</html>
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
