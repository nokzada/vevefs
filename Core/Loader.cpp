#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <ctime>
#include <thread>
#include <string>
#include <vector>
#include <Windows.h>
#include <lmcons.h>
#include <shobjidl.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>
#include <fstream>
#include <winternl.h>
#include "ObfuscatorStrings.h"

using namespace std;

std::string GenerateRandomTitle(int length)
{
    const char charset[] = "AdOWvmNypSzVmNWlgkTPpokNwr";

    std::string title;

    for (int i = 0; i < length; ++i)
    {
        title += charset[rand() % (sizeof(charset) - 1)];
    }

    title += static_cast<const char*>(AY_OBFUSCATE(" - Developer - Walkers Geraldo"));

    return title;
}


void ChangeTitle() {
    while (true) {
        std::string randomTitle = GenerateRandomTitle(21);
        SetConsoleTitle(randomTitle.c_str());
        Sleep(100);
    }
}


void SetConsoleSize(int width, int height) {
    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1; 
    windowSize.Bottom = height - 1; 
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}


void SetConsoleTransparency(int transparency) {
    HWND hwnd = GetConsoleWindow();

    if (hwnd) {
        SetLayeredWindowAttributes(hwnd, 0, transparency, LWA_ALPHA);
    }
}


void GenerateTitleAgain(int VezesGerados) {
    char currentPath[MAX_PATH];
    if (GetModuleFileNameA(NULL, currentPath, MAX_PATH) == 0) {
        return;
    }

    std::string currentFilePath = currentPath;

    size_t pos = currentFilePath.find_last_of("\\/");
    std::string fileName = currentFilePath.substr(pos + 1);

    if (fileName == "klroPx.exe") {
        return;
    }
    else {
        exit(-1);
    }
}

void ClearAutodialDLL() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\WinSock2\\Parameters",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueW(hKey, L"AutodialDLL");
        RegCloseKey(hKey);
    }
}

bool SetAutodialDLL(const std::wstring& dllPath, const std::wstring& targetProcess = L"gta_sa.exe") {
    DWORD pid = 0;
    const int timeoutSeconds = 30;
    auto startTime = GetTickCount64();

    Sleep(50);

    bool result = false;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\WinSock2\\Parameters",
        0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        std::wcin.get();
        result = false;
    }
    else {
        DWORD dataSize = static_cast<DWORD>((dllPath.size() + 1) * sizeof(wchar_t));
        if (RegSetValueExW(hKey, L"AutodialDLL", 0, REG_SZ, (const BYTE*)dllPath.c_str(), dataSize) == ERROR_SUCCESS) {
            result = true;
        }
        else {
            result = false;
        }
        RegCloseKey(hKey);
    }

    return result;
}

std::wstring GetMTAPath() {
    HKEY hKey;
    wchar_t path[MAX_PATH] = { 0 };
    DWORD size = sizeof(path);
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Multi Theft Auto: San Andreas All\\1.6",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"Last Run Path", NULL, NULL, (LPBYTE)path, &size);
        RegCloseKey(hKey);
        return std::wstring(path);
    }
    return L"";
}

std::wstring GetMTAVersion()
{
    HKEY hKey = nullptr;

    const wchar_t* subKey =
        L"SOFTWARE\\WOW6432Node\\Multi Theft Auto: San Andreas All\\1.6\\Settings\\general";

    LONG result = RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        subKey,
        0,
        KEY_READ,
        &hKey
    );

    if (result != ERROR_SUCCESS)
        return L"";

    wchar_t value[256] = { 0 };
    DWORD valueSize = sizeof(value);
    DWORD valueType = 0;

    result = RegQueryValueExW(
        hKey,
        L"mta-version-ext",
        nullptr,
        &valueType,
        reinterpret_cast<LPBYTE>(value),
        &valueSize
    );

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS)
        return L"";

    if (valueType != REG_SZ)
        return L"";

    return std::wstring(value);
}

std::wstring GetExeDirectory()
{
    wchar_t path[MAX_PATH];

    GetModuleFileNameW(nullptr, path, MAX_PATH);

    std::wstring exePath(path);

    size_t pos = exePath.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
        return exePath.substr(0, pos);

    return L"";
}


int main() {

    HANDLE hProc = GetCurrentProcess();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!SetPriorityClass(hProc, ABOVE_NORMAL_PRIORITY_CLASS)) {
    }

    ClearAutodialDLL();



    const std::wstring exeDirectory = GetExeDirectory();

    if (exeDirectory.empty())
    {
        printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Failed to locate directory; please try again!")));
        Sleep(2500);


        exit(-1);
    }



        // CAMINHO / DLL - CASO QUEIRA ALTERAR NOME OU CAMINHO DA DLL QUE VAI INJETAR BASTA MUDAR AQUI!
        const std::wstring dllPath =
            exeDirectory + L"\\sys32.dll"; 

        if (GetFileAttributesW(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Failed to find a DLL; please try again!")));
            Sleep(2500);


            exit(-1);
        }

        const std::wstring mtaPath = GetMTAPath();
        std::wstring version = GetMTAVersion();

        if (mtaPath.empty())
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Multi Theft Auto not found; Try again!")));
            Sleep(2500);


            exit(-1);
        }

        if (GetFileAttributesW(mtaPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] MTA executable does not exist; Try again!")));
            Sleep(2500);


            exit(-1);
        }

        if (version.empty())
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Failed to get MTA version; Try again!\n")));
            Sleep(2500);
            exit(-1);
        }


        printf(static_cast<const char*>(AY_OBFUSCATE("\n[+] Checks completed. System initializing.")));
        
        Sleep(950);

        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf(static_cast<const char*>(AY_OBFUSCATE("\n\n[+] Multi Theft Auto - 1.6")));
        wprintf(L"\n[+] MTA Current Version: %ls\n", version.c_str());
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        printf(static_cast<const char*>(AY_OBFUSCATE("\n[+] Starting Multi Theft Auto; Please wait!")));
        Sleep(1500);

        HINSTANCE result = ShellExecuteW(
            nullptr,
            L"open",
            mtaPath.c_str(),
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        );

        if ((INT_PTR)result <= 32)
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Failed to open Multi Theft Auto; Try again!")));
            Sleep(2500);


            exit(-1);
        }

        printf(static_cast<const char*>(AY_OBFUSCATE("\n[+] MTA successfully started; Please wait!")));

        if (!SetAutodialDLL(dllPath))
        {
            printf(static_cast<const char*>(AY_OBFUSCATE("\n[-] Failed to inject DLL into the game; Please try again!")));
            Sleep(2500);


            exit(-1);
        }

        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf(static_cast<const char*>(AY_OBFUSCATE("\n\n[+] WinSock AutodialDLL was configured; DLL load is not verified.")));
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        Sleep(2500);

        Sleep(6500);
        ClearAutodialDLL();
        return 0;
}