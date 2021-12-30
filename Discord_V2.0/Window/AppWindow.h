#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <dwrite_3.h>
#include "WindowElements.h"
#define DARK_THEME

class AppWindow
{
	friend class Application;
private:
	LPCWSTR MainClassName = L"MainWindowClass";
	HWND MainWindow;

	COLORREF wBG;
	int wWidth, wHeight; // Window resolution
	short wPosX, wPosY;  // Window position
	int sWidth, sHeight; // Screen resolution

	Button CloseButton;
	Button UpButton;
	Button DownButton;

	Label Title;

	BOOL Initialized = false;

	// Direct2D stuff
	ID2D1Factory* pD2DFactory;
	IDWriteFactory7* pDWFactory;
	ID2D1HwndRenderTarget* pRT;
	ID2D1SolidColorBrush* pBG;

private:
	void CenterWindow(HWND hWnd);
	void UpdateAppWindow(HWND hWnd);
	void CloseAppWindow(HWND hWnd);
	void Render();
	void CreateAllWindowElements();
	void Initialize();
	
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	AppWindow() = default;
	AppWindow(HINSTANCE hInstance, int* w, int* h);
	~AppWindow();

	BOOL CreateAppWindow(HINSTANCE hInstance, int* w, int* h);
	void ShowAppWindow(int nCmdShow);
	int StartReadingMessages();
};