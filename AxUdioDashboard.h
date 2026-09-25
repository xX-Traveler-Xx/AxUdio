#ifndef AXUDIO_DASHBOARD_H
#define AXUDIO_DASHBOARD_H

#include <string_view>

// Вшиваем весь HTML-код страницы внутрь бинарника библиотеки
inline constexpr std::string_view g_AxUdioDashboardHTML = R"html(
<!DOCTYPE html>
<html lang="ru">
<head>
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
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
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
)html";

#endif // AXUDIO_DASHBOARD_H