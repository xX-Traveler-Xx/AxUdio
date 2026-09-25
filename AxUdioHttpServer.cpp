#include "AxUdioCore.h"
#include "AxUdioDashboard.h"
#include <thread>
#include <atomic>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cstdlib>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

// Кроссплатформенная функция автоматического открытия системного браузера
static void OpenInBrowser(const std::string& url) {
#if defined(_WIN32)
    // Открывает URL через Shell API без всплывающего окна cmd
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    std::string command = "open " + url;
    std::system(command.c_str());
#elif defined(__ANDROID__)
    // В Termux / Android Linux-окружениях
    std::string command = "termux-open-url " + url;
    std::system(command.c_str());
#else
    // Для Linux (Ubuntu, Debian, Fedora и др.)
    std::string command = "xdg-open " + url + " > /dev/null 2>&1 &";
    std::system(command.c_str());
#endif
}

class AxUdioWebServer {
private:
    socket_t serverFd = INVALID_SOCKET;
    std::atomic<bool> isRunning{ false };
    std::thread serverThread;

public:
    void Start(int port = 8080) {
#if defined(_WIN32)
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == INVALID_SOCKET) {
            std::cout << "[AxUdio Server] Ошибка создания сокета!\n";
            return;
        }

        int opt = 1;
        setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(serverFd, (sockaddr*)&address, sizeof(address)) < 0) {
            std::cout << "[AxUdio Server] Ошибка bind (порт " << port << " занят)!\n";
            closesocket(serverFd);
            return;
        }

        if (listen(serverFd, 10) < 0) {
            std::cout << "[AxUdio Server] Ошибка listen!\n";
            closesocket(serverFd);
            return;
        }

        isRunning = true;
        std::cout << "[AxUdio Server] Сервер запущен! Откройте браузер по адресу: http://localhost:" << port << "\n";
        serverThread = std::thread(&AxUdioWebServer::ListenLoop, this);
    }

    void ListenLoop() {
        while (isRunning) {
            sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            socket_t clientSocket = accept(serverFd, (sockaddr*)&clientAddr, &addrLen);

            if (clientSocket == INVALID_SOCKET || !isRunning) {
                if (clientSocket != INVALID_SOCKET) closesocket(clientSocket);
                break;
            }

            char buffer[2048] = { 0 };
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

            if (bytesReceived > 0) {
                std::string request(buffer);
                std::ostringstream response;

                // 1. Отдача JSON-метрики для Дашборда
                if (request.find("GET /api/stats") != std::string::npos) {
                    std::string json = "{\"rms\":0.35,\"peak\":0.72,\"bufferSize\":1024,\"driver\":\"DirectSound/ALSA\"}";
                    response << "HTTP/1.1 200 OK\r\n"
                        << "Content-Type: application/json\r\n"
                        << "Access-Control-Allow-Origin: *\r\n"
                        << "Content-Length: " << json.size() << "\r\n"
                        << "Connection: close\r\n\r\n"
                        << json;
                }
                // 2. Отдача главной HTML страницы
                else {
                    response << "HTTP/1.1 200 OK\r\n"
                        << "Content-Type: text/html; charset=utf-8\r\n"
                        << "Content-Length: " << g_AxUdioDashboardHTML.size() << "\r\n"
                        << "Connection: close\r\n\r\n"
                        << g_AxUdioDashboardHTML;
                }

                std::string respStr = response.str();

                send(clientSocket, respStr.c_str(), static_cast<int>(respStr.size()), 0);

#if defined(_WIN32)
                shutdown(clientSocket, SD_SEND);
#else
                shutdown(clientSocket, SHUT_WR);
#endif
            }

            closesocket(clientSocket);
        }
    }

    void Stop() {
        if (!isRunning) return;
        isRunning = false;

        if (serverFd != INVALID_SOCKET) {
            closesocket(serverFd);
            serverFd = INVALID_SOCKET;
        }

        if (serverThread.joinable()) {
            serverThread.join();
        }

#if defined(_WIN32)
        WSACleanup();
#endif
    }

    ~AxUdioWebServer() {
        Stop();
    }
};

static AxUdioWebServer g_webServer;

void StartAxUdioDashboard(int port) {
    g_webServer.Start(port);

    // Автоматическое открытие дашборда в браузере по умолчанию
    std::string url = "http://localhost:" + std::to_string(port);
    OpenInBrowser(url);
}