#ifndef MAINWINDOW_HEADER
#define MAINWINDOW_HEADER
#include <Windows.h>
#include "../Console/Console.h"

static LPCWSTR MainClassName = L"MainWindowClass";
static HWND MainWindow;
int wWidth, wHeight; // Window resolution
short wPosX, wPosY;  // Window position
int sWidth, sHeight; // Screen resolution

static void CenterWindow(HWND hWnd)
{
	RECT rc = { 0 };

	GetWindowRect(hWnd, &rc);
	int win_w = rc.right - rc.left;
	int win_h = rc.bottom - rc.top;

	sWidth = GetSystemMetrics(SM_CXSCREEN);
	sHeight = GetSystemMetrics(SM_CYSCREEN);

	wPosX = (sWidth - win_w) / 2;
	wPosY = (sHeight - win_h) / 2;

	SetWindowPos(hWnd, HWND_TOP, wPosX,
		wPosY, 0, 0, SWP_NOSIZE);

#ifdef _DEBUG
	AllocateConsoles();
#endif
}

static void CloseAppWindow(HWND hWnd)
{
	DeallocateConsoles();
	DestroyWindow(hWnd);
	PostQuitMessage(0);
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
		CloseAppWindow(hWnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			CloseAppWindow(hWnd);
		break;
	case WM_NCHITTEST:
		LRESULT hit = DefWindowProcW(hWnd, Msg, wParam, lParam);
		if (hit == HTCLIENT)
		{
			POINT p;
			p.x = (short)lParam;
			p.y = (short)(lParam >> 16);
			ScreenToClient(hWnd, &p);
			if (p.y <= 30)
				hit = HTCAPTION;
		}
		return hit;
	}
	return DefWindowProcW(hWnd, Msg, wParam, lParam);
}


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

	wWidth, wHeight;

	if (w)
		wWidth = *w;
	else
		wWidth = 800;
	if (h)
		wHeight = *h;
	else
		wHeight = 600;
	
	MainWindow = CreateWindowExW(
		WS_EX_ACCEPTFILES | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		MainClassName,
		L"Discord 2.0",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, wWidth, wHeight,
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