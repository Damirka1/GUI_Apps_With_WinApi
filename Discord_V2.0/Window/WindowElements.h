#pragma once

struct Button
{
	int x, y;
	int width, height;
	BOOL Hover;
	COLORREF Color;
	COLORREF HoverColor;
	ID2D1SolidColorBrush* pColor;
	ID2D1SolidColorBrush* pHoverColor;

};

struct Label
{
	LPCWSTR text;
	int x, y;
	int width, height;
	IDWriteTextFormat* pFormat;
};
