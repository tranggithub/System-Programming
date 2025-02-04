#include <Windows.h>
#include <iostream>

typedef HANDLE(WINAPI* pCreateFileW)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

pCreateFileW OriginalCreateFileW = NULL;

HANDLE WINAPI MyCreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
) {
    // Ki?m tra file "C:\\1.txt"
    if (wcscmp(lpFileName, L"C:\\1.txt") == 0) {
        std::cout << "Warning: Attempt to open file C:\\1.txt" << std::endl;
    }

    return OriginalCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void HookIAT() {
    // L?y ??a ch? c?a module kernel32.dll
    HMODULE hModule = GetModuleHandle(L"kernel32.dll");

    // Tìm ??a ch? hàm CreateFileW trong kernel32.dll
    FARPROC pCreateFile = GetProcAddress(hModule, "CreateFileW");

    // L?y thông tin b? nh? cho IAT
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)pDosHeader + pDosHeader->e_lfanew);

    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress + (DWORD_PTR)pDosHeader);

    while (pImportDesc->Name) {
        PSTR pszModName = (PSTR)((DWORD_PTR)pDosHeader + pImportDesc->Name);
        if (_stricmp(pszModName, "kernel32.dll") == 0) {
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)pDosHeader + pImportDesc->FirstThunk);

            // Tìm v? trí c?a CreateFileW trong b?ng IAT và thay th?
            while (pThunk->u1.Function) {
                if (pThunk->u1.Function == (DWORD_PTR)pCreateFile) {
                    // L?u l?i ??a ch? c?a hàm g?c
                    OriginalCreateFileW = (pCreateFileW)pThunk->u1.Function;

                    // S? d?ng VirtualProtect ?? thay ??i quy?n ghi cho vùng b? nh? ch?a IAT
                    DWORD oldProtect;
                    if (VirtualProtect(&pThunk->u1.Function, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                        pThunk->u1.Function = (DWORD_PTR)MyCreateFileW; // Thay th? v?i MyCreateFileW
                        VirtualProtect(&pThunk->u1.Function, sizeof(void*), oldProtect, &oldProtect); // Khôi ph?c quy?n truy c?p
                    }
                    break;
                }
                pThunk++;
            }
            break;
        }
        pImportDesc++;
    }
}

int main() {
    // G?n hook vào CreateFile
    HookIAT();

    while (true)
    {
        Sleep(1000);
    }
    /*
    // Th? m? file
    HANDLE hFile = CreateFileW(L"C:\\1.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        std::cout << "File opened successfully." << std::endl;
        CloseHandle(hFile);
    }*/

    return 0;
}