#ifndef MAINWINDOW_HEADER
#define MAINWINDOW_HEADER
#include <Windows.h>
#include <stdio.h>

static void CenterWindow(HWND hWnd)
{
	RECT rc = { 0 };

	GetWindowRect(hWnd, &rc);
	int win_w = rc.right - rc.left;
	int win_h = rc.bottom - rc.top;

	int screen_w = GetSystemMetrics(SM_CXSCREEN);
	int screen_h = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(hWnd, HWND_TOP, (screen_w - win_w) / 2,
		(screen_h - win_h) / 2, 0, 0, SWP_NOSIZE);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam,  LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		CenterWindow(hWnd);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

static LPCWSTR MainClassName = L"MainWindowClass";
static HWND MainWindow;

BOOL CreateAppWindow(HINSTANCE hInstance, int* w, int* h)
{
	WNDCLASSEXW WndClass;
	memset(&WndClass, 0, sizeof(WndClass));
	//WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.hInstance = hInstance;
	WndClass.lpszClassName = MainClassName;
	WndClass.cbSize = sizeof(WndClass);
#ifdef DARK_THEME
	WndClass.hbrBackground = CreateSolidBrush(RGB(23, 26, 33));
#else
	WndClass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
#endif
	WndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	WndClass.lpfnWndProc = WndProc;

	RegisterClassExW(&WndClass);

	int Width, Height;

	if (w)
		Width = *w;
	else
		Width = 800;
	if (h)
		Height = *h;
	else
		Height = 600;
	
	MainWindow = CreateWindowExW(
		WS_EX_ACCEPTFILES | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		MainClassName,
		L"Discord 2.0",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!MainWindow)
		return FALSE;
	return TRUE;
}

void ShowAppWindow(int nCmdShow)
{
	ShowWindow(MainWindow, nCmdShow);
	UpdateWindow(MainWindow);
}

int StartReadingMessages()
{
	MSG Msg;

	while (GetMessageW(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}
	return (int)Msg.wParam;
}

#endif