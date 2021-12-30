#pragma once
#include "../Window/AppWindow.h"

class Application
{
	AppWindow Window;

public:
	Application(HINSTANCE hInstance, int* w, int* h);
	int Run(int nCmdShow);
};
