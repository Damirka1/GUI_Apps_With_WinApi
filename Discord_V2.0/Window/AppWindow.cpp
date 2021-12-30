#include "..\Window\AppWindow.h"
#include "../Console/Console.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

LRESULT CALLBACK AppWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		AppWindow* pWnd = static_cast<AppWindow*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&AppWindow::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK AppWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window instance
	AppWindow* pWnd = reinterpret_cast<AppWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

void AppWindow::CenterWindow(HWND hWnd)
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
}

void AppWindow::UpdateAppWindow(HWND hWnd)
{
	InvalidateRect(hWnd, NULL, TRUE);
}

void AppWindow::CloseAppWindow(HWND hWnd)
{
#ifdef _DEBUG
	DeallocateConsoles();
#endif
	DestroyWindow(hWnd);
	PostQuitMessage(0);
}

void AppWindow::CreateAllWindowElements()
{
	// Create close button
	CloseButton.Color = RGB(242, 108, 79);
	CloseButton.HoverColor = RGB(255, 255, 255);
	pRT->CreateSolidColorBrush(D2D1::ColorF(0.949f, 0.423f, 0.309f, 1), &CloseButton.pColor);
	pRT->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1 ), &CloseButton.pHoverColor);
	CloseButton.width = 10;
	CloseButton.height = CloseButton.width;
	CloseButton.x = wWidth - CloseButton.width;
	CloseButton.y = CloseButton.width;
	CloseButton.Hover = FALSE;

	// Create up button
	UpButton.Color = RGB(200, 200, 200);
	UpButton.HoverColor = RGB(255, 255, 255);
	pRT->CreateSolidColorBrush(D2D1::ColorF(0.784f, 0.784f, 0.784f), &UpButton.pColor);
	UpButton.pHoverColor = CloseButton.pHoverColor;
	UpButton.width = CloseButton.width - 2;
	UpButton.height = UpButton.width + 2;
	UpButton.x = CloseButton.x - (int)(CloseButton.width * 2.5);
	UpButton.y = UpButton.height;
	UpButton.Hover = FALSE;

	// Create down button
	DownButton.Color = RGB(200, 200, 200);
	DownButton.HoverColor = RGB(255, 255, 255);
	DownButton.pColor = UpButton.pColor;
	DownButton.pHoverColor = UpButton.pHoverColor;
	DownButton.width = UpButton.width;
	DownButton.height = 1;
	DownButton.x = UpButton.x - UpButton.width * 3;
	DownButton.y = UpButton.y + UpButton.height - DownButton.height;
	DownButton.Hover = FALSE;

	// Title label
	Title.text = L"Discord 2.0";
	Title.x = 5;
	Title.y = 5;
	Title.width = 100;
	Title.height = 30;
	pDWFactory->CreateTextFormat(
		L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &Title.pFormat
	);
}

void AppWindow::Initialize()
{
	CenterWindow(MainWindow);

	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&pD2DFactory
	);

	hr = pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			MainWindow,
			D2D1::SizeU(
				wWidth,
				wHeight)
		),
		&pRT
	);

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWFactory)
	);

	if (SUCCEEDED(hr))
	{
		CreateAllWindowElements();
		Initialized = true;
	}
}

void AppWindow::Render()
{
	pRT->BeginDraw();
	pRT->Clear(D2D1::ColorF(0.09f, 0.101f,  0.129f, 1.0f));

	// Render close button
	{
		pRT->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(CloseButton.x - CloseButton.width), static_cast<FLOAT>(CloseButton.y / 2)),
			D2D1::Point2F(static_cast<FLOAT>(CloseButton.x + CloseButton.width / 2), static_cast<FLOAT>(CloseButton.y + CloseButton.height)),
			CloseButton.Hover ? CloseButton.pHoverColor : CloseButton.pColor,
			2
		);

		pRT->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(CloseButton.x + CloseButton.width / 2), static_cast<FLOAT>(CloseButton.y / 2)),
			D2D1::Point2F(static_cast<FLOAT>(CloseButton.x - CloseButton.width), static_cast<FLOAT>(CloseButton.y + CloseButton.height)),
			CloseButton.Hover ? CloseButton.pHoverColor : CloseButton.pColor,
			2
		);
	}

	// Render up button
	{
		pRT->DrawRectangle(
			D2D1::RectF(
				static_cast<FLOAT>(UpButton.x - UpButton.width),
				static_cast<FLOAT>(UpButton.y),
				static_cast<FLOAT>(UpButton.x + UpButton.width),
				static_cast<FLOAT>(UpButton.y + UpButton.height)),
			UpButton.Hover ? UpButton.pHoverColor : UpButton.pColor,
			1.5f);
	}

	// Render down button
	{

		pRT->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(DownButton.x - DownButton.width), static_cast<FLOAT>(DownButton.y)),
			D2D1::Point2F(static_cast<FLOAT>(DownButton.x + DownButton.width), static_cast<FLOAT>(DownButton.y)),
			DownButton.Hover ? DownButton.pHoverColor : DownButton.pColor,
			1.5f
		);
	}

	// Render title
	{
		pRT->DrawTextW(Title.text, lstrlenW(Title.text), Title.pFormat,
			D2D1_RECT_F(Title.x, Title.y, Title.x + Title.width, Title.y + Title.height), DownButton.pColor);
	}

	HRESULT hr = pRT->EndDraw();
}

LRESULT AppWindow::HandleMsg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		CloseAppWindow(hWnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			CloseAppWindow(hWnd);
		break;

	case WM_LBUTTONDOWN:
		if (CloseButton.Hover)
			CloseAppWindow(hWnd);
		break;
	case WM_PAINT:
		Render();
		break;

	case WM_MOUSEMOVE:
		POINT p;
		p.x = (short)lParam;
		p.y = (short)(lParam >> 16);

		if (p.y <= 25)
		{
			if (p.x >= CloseButton.x - CloseButton.width)
			{
				CloseButton.Hover = TRUE;
				UpdateAppWindow(hWnd);
			}
			else
			{
				CloseButton.Hover = FALSE;
				UpdateAppWindow(hWnd);
			}

			if (p.x >= UpButton.x - UpButton.width && p.x < CloseButton.x - CloseButton.width)
			{
				UpButton.Hover = TRUE;
				UpdateAppWindow(hWnd);
			}
			else
			{
				UpButton.Hover = FALSE;
				UpdateAppWindow(hWnd);
			}

			if (p.x >= DownButton.x - DownButton.width && p.x < UpButton.x - UpButton.width)
			{
				DownButton.Hover = TRUE;
				UpdateAppWindow(hWnd);
			}
			else
			{
				DownButton.Hover = FALSE;
				UpdateAppWindow(hWnd);
			}
		}
		else
		{
			if (CloseButton.Hover || UpButton.Hover || DownButton.Hover)
			{
				CloseButton.Hover = FALSE;
				UpButton.Hover = FALSE;
				DownButton.Hover = FALSE;
				UpdateAppWindow(hWnd);
			}
		}
		break;
	case WM_NCHITTEST:
	{
		LRESULT hit = DefWindowProcW(hWnd, Msg, wParam, lParam);
		if (hit == HTCLIENT)
		{
			POINT p;
			p.x = (short)lParam;
			p.y = (short)(lParam >> 16);
			ScreenToClient(hWnd, &p);

			if (p.x > wWidth - (CloseButton.width + UpButton.width + DownButton.width) * 3)
				return hit;

			// Move window
			if (p.y <= 30)
				hit = HTCAPTION;
		}
		return hit;
	}
	default:
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}
	return true;
}

AppWindow::AppWindow(HINSTANCE hInstance, int* w, int* h)
{
	CreateAppWindow(hInstance, w, h);
}

BOOL AppWindow::CreateAppWindow(HINSTANCE hInstance, int* w, int* h)
{
#ifdef _DEBUG
	AllocateConsoles();
#endif

	WNDCLASSEXW WndClass;
	memset(&WndClass, 0, sizeof(WndClass));
	//WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.hInstance = hInstance;
	WndClass.lpszClassName = MainClassName;
	WndClass.cbSize = sizeof(WndClass);
	//WndClass.cbWndExtra = sizeof(LONG_PTR);
#ifdef DARK_THEME
	wBG = RGB(23, 26, 33);
#else
	WndClass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
#endif
	WndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	WndClass.lpfnWndProc = AppWindow::HandleMsgSetup;

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
		this);

	if (!MainWindow)
		return FALSE;
	return TRUE;
}

void AppWindow::ShowAppWindow(int nCmdShow)
{
	if (!Initialized)
		Initialize();

	ShowWindow(MainWindow, nCmdShow);
	UpdateWindow(MainWindow);
}

int AppWindow::StartReadingMessages()
{
	MSG Msg;

	while (GetMessageW(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}
	return (int)Msg.wParam;
}

AppWindow::~AppWindow()
{
	pD2DFactory->Release();
	pRT->Release();

	CloseButton.pColor->Release();
	CloseButton.pHoverColor->Release();
	
	UpButton.pColor->Release();
}
