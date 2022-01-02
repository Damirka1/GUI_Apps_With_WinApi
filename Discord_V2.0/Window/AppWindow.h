#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include "WindowElements.h"

#include <vector>

#define DARK_THEME

class AppWindow
{
	friend class Application;
private:
	LPCWSTR MainClassName = L"MainWindowClass";
	HWND MainWindow;

	COLORREF wBG;
	int wWidth, wHeight; // Window resolution
	int wPosX, wPosY;  // Window position
	int sWidth, sHeight; // Screen resolution

	int cPosX, cPosY;	// Position for child window
	int	cWidth, cHeight; // Resolution of child window

	Button CloseButton;
	Button UpButton;
	Button DownButton;
	Label Title;

	BOOL Initialized = false;

	// Direct2D stuff
	ID2D1Factory* pD2DFactory;
	IDWriteFactory7* pDWFactory;
	ID2D1HwndRenderTarget* pRT; // Render target
	ID2D1SolidColorBrush* pBG;  // brush for window
	ID2D1SolidColorBrush* pCBG; // brush for child window

	IWICImagingFactory* d2dWICFactory;
	IWICBitmapDecoder* d2dDecoder;
	IWICFormatConverter* d2dConverter;
	IWICBitmapFrameDecode* d2dBmpSrc;
	std::vector<ID2D1Bitmap*> BitMaps;

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