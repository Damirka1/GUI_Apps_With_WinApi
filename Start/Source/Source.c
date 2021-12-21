#include <windows.h>
#include "../resource.h"

const wchar_t g_szClassName[] = L"myWindowClass";


LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:
            MessageBoxW(hwnd, L"Hi!", L"This is a message",
                MB_OK | MB_ICONEXCLAMATION);
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;
    case WM_INITDIALOG:

        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_HELLO:
        {
            int ret = DialogBoxW(GetModuleHandle(NULL),
                MAKEINTRESOURCE(IDD_DIALOG1), hwnd, AboutDlgProc);
            if (ret == IDOK) {
                MessageBoxW(hwnd, L"Dialog exited with IDOK.", L"Notice",
                    MB_OK | MB_ICONINFORMATION);
            }
            else if (ret == IDCANCEL) {
                MessageBoxW(hwnd, L"Dialog exited with IDCANCEL.", L"Notice",
                    MB_OK | MB_ICONINFORMATION);
            }
            else if (ret == 0) {
                MessageBoxW(hwnd, L"Dialog closed!", L"Notice",
                    MB_OK | MB_ICONINFORMATION);
            }
            else if (ret == -1) {
                MessageBoxW(hwnd, L"Dialog failed!", L"Error",
                    MB_OK | MB_ICONINFORMATION);
            }
        }
        break;
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    WNDCLASSEXW wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_ICON1));;

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Hello, world!",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hwnd)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return (int)Msg.wParam;
}