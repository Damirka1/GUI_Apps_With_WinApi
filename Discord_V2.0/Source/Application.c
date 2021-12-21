#define DARK_THEME
#include "../Window/MainWindow.h"


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	int w = 1280;
	int h = 720;

	CreateAppWindow(hInstance, &w, &h);
	ShowAppWindow(nCmdShow);
	return StartReadingMessages();
}