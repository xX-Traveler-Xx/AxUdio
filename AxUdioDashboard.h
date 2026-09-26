#pragma once

#ifndef AXUDIO_DASHBOARD_H
#define AXUDIO_DASHBOARD_H

#include <string_view>

// Вшиваем весь HTML-код страницы внутрь бинарника библиотеки

    inline constexpr std::string_view g_AxUdioDashboardHTML = (const char*)u8R"html(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>===[ AxUdio Audio Engine Control Panel v1.0.9 (c) 2007 Underground Sound System ]===</title>
    <style>
        :root {
            --bg-color: #050805;
            --card-bg: #0b100b;
            --accent-green: #00ff00;
            --accent-yellow: #ffff00;
            --accent-red: #ff0000;
            --text-color: #00ff00;
            --text-dim: #00aa00;
            --border-color: #00ff00;
        }

        body {
            background-color: var(--bg-color);
            background-image: 
                linear-gradient(rgba(0, 255, 0, 0.05) 50%, rgba(0, 0, 0, 0.35) 50%),
                radial-gradient(#002200, #000000);
            background-size: 100% 4px, cover;
            color: var(--text-color);
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 15px;
            display: flex;
            justify-content: center;
            text-shadow: 0 0 5px rgba(0, 255, 0, 0.7);
        }

        .container {
            width: 100%;
            max-width: 850px;
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            border: 3px double var(--accent-green);
            background: rgba(5, 12, 5, 0.95);
            padding: 15px;
            box-shadow: 0 0 20px rgba(0, 255, 0, 0.4), inset 0 0 15px #000;
        }

        .header {
            grid-column: 1 / -1;
            background: #000000;
            padding: 12px;
            border: 2px solid #005500;
            border-bottom: 2px solid var(--accent-green);
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        h1 {
            margin: 0;
            font-size: 16px;
            letter-spacing: 2px;
            color: #00ff00;
            font-weight: bold;
        }

        .marquee-line {
            grid-column: 1 / -1;
            background: #001100;
            color: var(--accent-yellow);
            border: 1px dashed var(--accent-green);
            font-size: 11px;
            padding: 3px;
        }

        .card {
            background: var(--card-bg);
            padding: 12px;
            border: 2px solid #004400;
            box-shadow: 3px 3px 0px #000;
        }

        .full-width {
            grid-column: 1 / -1;
        }

        h2 {
            margin-top: 0;
            font-size: 12px;
            text-transform: uppercase;
            color: #ffffff;
            background: #003300;
            padding: 4px 8px;
            border-left: 4px solid var(--accent-green);
            letter-spacing: 1px;
        }

        /* Индикаторы уровней */
        .meter-container {
            margin-bottom: 15px;
        }

        .meter-label {
            display: flex;
            justify-content: space-between;
            font-size: 11px;
            margin-bottom: 3px;
            color: #aaffaa;
        }

        .meter-bar {
            height: 16px;
            background: #000;
            border: 1px solid #006600;
            padding: 1px;
            position: relative;
        }

        .meter-fill {
            height: 100%;
            width: 0%;
            transition: width 0.05s steps(20);
        }

        .fill-rms {
            background: linear-gradient(90deg, #005500, var(--accent-green));
        }

        .fill-peak {
            background: linear-gradient(90deg, #888800, var(--accent-yellow));
        }

        /* Контролы */
        .control-group {
            margin-bottom: 12px;
        }

        label {
            display: block;
            font-size: 11px;
            margin-bottom: 5px;
            color: #00ff00;
        }

        input[type="range"] {
            width: 100%;
            accent-color: var(--accent-green);
            background: #000;
        }

        button {
            background: linear-gradient(180deg, #222222 0%, #000000 100%);
            color: #00ff00;
            border: 2px outset #00aa00;
            padding: 6px 12px;
            cursor: pointer;
            font-weight: bold;
            font-family: 'Courier New', Courier, monospace;
            font-size: 11px;
            text-transform: uppercase;
            box-shadow: 2px 2px 0px #000;
        }

        button:active {
            border-style: inset;
            background: #000;
            color: #fff;
        }

        button:hover {
            background: #003300;
            color: #ffffff;
            border-color: var(--accent-green);
        }

        /* Консоль */
        .console-log {
            background: #000000;
            border: 1px solid var(--accent-green);
            padding: 8px;
            height: 120px;
            overflow-y: auto;
            font-family: 'Courier New', Courier, monospace;
            font-size: 11px;
            color: var(--accent-green);
            box-shadow: inset 0 0 8px rgba(0, 255, 0, 0.5);
        }

        .log-entry {
            margin-bottom: 2px;
        }

        .log-error {
            color: var(--accent-red);
            font-weight: bold;
        }

        .log-warn {
            color: var(--accent-yellow);
        }

        /* Сетка статуса */
        .status-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 8px;
            font-size: 11px;
        }

        .status-item {
            background: #000000;
            border: 1px solid #004400;
            padding: 8px;
            text-align: center;
        }

        .status-value {
            font-size: 13px;
            font-weight: bold;
            color: #ffff00;
            margin-top: 4px;
        }

        .blink {
            animation: blinker 1s linear infinite;
        }

        @keyframes blinker {
            50% { opacity: 0; }
        }
    </style>
</head>
<body>

    <div class="container">

        <!-- Хедер -->
        <div class="header">
            <div>
                <h1>[ AXUDIO ENGINE MONITOR v1.0 ]</h1>
                <small style="color: var(--text-dim)">// Underground DSP Audio Core</small>
            </div>
            <div>
                <span id="conn-status" class="blink" style="color: var(--accent-red); font-size: 11px; font-weight: bold;">[● OFFLINE]</span>
            </div>
        </div>

        <!-- Бегущая строка -->
        <div class="marquee-line">
            <marquee scrollamount="4">+++ AxUdio Audio System loaded... Ready for processing... Respect to underground coders! +++</marquee>
        </div>

        <!-- Монитор Уровней (RMS / PEAK) -->
        <div class="card">
            <h2>> VU Meters</h2>

            <div class="meter-container">
                <div class="meter-label">
                    <span>RMS (AVERAGE)</span>
                    <span id="val-rms">0.00</span>
                </div>
                <div class="meter-bar">
                    <div id="bar-rms" class="meter-fill fill-rms"></div>
                </div>
            </div>

            <div class="meter-container">
                <div class="meter-label">
                    <span>PEAK (MAX GAIN)</span>
                    <span id="val-peak">0.00</span>
                </div>
                <div class="meter-bar">
                    <div id="bar-peak" class="meter-fill fill-peak"></div>
                </div>
            </div>
        </div>

        <!-- Управление движком -->
        <div class="card">
            <h2>> Controls</h2>

            <div class="control-group">
                <label for="volume-slider">MASTER VOL: <span id="val-vol">100%</span></label>
                <input type="range" id="volume-slider" min="0" max="100" value="100" oninput="updateVolume(this.value)">
            </div>

            <div class="control-group" style="display: flex; gap: 8px;">
                <button onclick="sendCommand('pause')">[ PLAY/PAUSE ]</button>
                <button onclick="sendCommand('flush')">[ FLUSH BUF ]</button>
            </div>
        </div>

        <!-- Метрики PCM и Железа -->
        <div class="card full-width">
            <h2>> PCM Status & Hardware Info</h2>
            <div class="status-grid">
                <div class="status-item">
                    <div>SAMPLE RATE</div>
                    <div class="status-value" id="val-rate">44100 Hz</div>
                </div>
                <div class="status-item">
                    <div>RING BUFFER</div>
                    <div class="status-value" id="val-buffer">0 smpl</div>
                </div>
                <div class="status-item">
                    <div>DRIVER ID</div>
                    <div class="status-value" id="val-driver">INIT...</div>
                </div>
            </div>
        </div>

        <!-- Консоль логов и ошибок -->
        <div class="card full-width">
            <h2>> System Log Terminal</h2>
            <div class="console-log" id="console-log">
                <div class="log-entry">[SYS] AxUdio Terminal Initialized...</div>
            </div>
        </div>

    </div>

    <script>
        const WS_URL = "ws://" + window.location.hostname + ":" + (window.location.port || "8080") + "/axudio";
        let ws = null;
        let useHttpFallback = false;
        let pollTimer = null;

        function updateUI(data) {
            const statusEl = document.getElementById('conn-status');
            statusEl.innerText = '[● ONLINE]';
            statusEl.style.color = 'var(--accent-green)';
            statusEl.classList.remove('blink');

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
                    document.getElementById('bar-peak').style.background = 'linear-gradient(90deg, #888800, var(--accent-yellow))';
                }
            }

            if (data.bufferSize !== undefined) document.getElementById('val-buffer').innerText = data.bufferSize + ' smpl';
            if (data.driver !== undefined) document.getElementById('val-driver').innerText = data.driver;
            if (data.sampleRate !== undefined) document.getElementById('val-rate').innerText = data.sampleRate + ' Hz';

            if (data.error) {
                addLog('[ERR] ' + data.error, 'error');
            }
        }

        function startHttpPolling() {
            if (pollTimer) return;
            useHttpFallback = true;
            addLog('[HTTP] Switching to REST API Fallback...', 'warn');

            pollTimer = setInterval(() => {
                fetch('/api/stats')
                    .then(res => res.json())
                    .then(data => updateUI(data))
                    .catch(() => {
                        const statusEl = document.getElementById('conn-status');
                        statusEl.innerText = '[● OFFLINE]';
                        statusEl.style.color = 'var(--accent-red)';
                        statusEl.classList.add('blink');
                    });
            }, 50);
        }

        function connect() {
            try {
                ws = new WebSocket(WS_URL);

                ws.onopen = () => {
                    addLog('[WS] Connection established with C++ Engine.');
                    if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
                    useHttpFallback = false;
                };

                ws.onmessage = (event) => {
                    try {
                        const data = JSON.parse(event.data);
                        updateUI(data);
                    } catch (e) {
                        addLog('[WS] JSON Parse Error', 'error');
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
                addLog('[CMD WS] Sent: ' + cmd);
            } else {
                fetch('/api/command', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(msg)
                })
                .then(() => addLog('[CMD HTTP] Sent: ' + cmd))
                .catch(() => addLog('[ERR] Failed to send command', 'error'));
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

        // Autostart
        connect();
    </script>

</body>
</html>
)html";

#endif // AXUDIO_DASHBOARD_H