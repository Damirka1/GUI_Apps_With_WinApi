#ifndef CONSOLE_HEADER
#define CONSOLE_HEADER
#include <Windows.h>

static HANDLE ConIn;
static HANDLE ConOut;

void AllocateConsoles()
{
	AllocConsole();

	ConIn = GetStdHandle(STD_INPUT_HANDLE);
	ConOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTitleW(L"Discrod 2.0 Debug Console");

	
}

void DeallocateConsoles()
{
	FreeConsole();
}

void PrintInfo(LPCWSTR Msg)
{
	SetConsoleTextAttribute(ConOut, 7 | 0); // white text, black background
	if (Msg)
		WriteConsoleW(ConOut, Msg, lstrlenW(Msg), NULL, NULL);
}

void PrintWarning(LPCWSTR Msg)
{
	SetConsoleTextAttribute(ConOut, 6 | 0); // yellow text, black background
	if (Msg)
		WriteConsoleW(ConOut, Msg, lstrlenW(Msg), NULL, NULL);
}

void PrintError(LPCWSTR Msg)
{
	SetConsoleTextAttribute(ConOut, 4 | 0); // red text, black background
	if (Msg)
		WriteConsoleW(ConOut, Msg, lstrlenW(Msg), NULL, NULL);
}

#endif
