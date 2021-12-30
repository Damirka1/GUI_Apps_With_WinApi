#include "Application.h"

Application::Application(HINSTANCE hInstance, int* w, int* h)
	:
	Window(hInstance, w, h)
{
}

int Application::Run(int nCmdShow)
{
	Window.ShowAppWindow(nCmdShow);
	return Window.StartReadingMessages();
}
