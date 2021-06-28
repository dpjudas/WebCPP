#pragma once

#include <cmath>
#include <Windows.h>

enum class WindowShadowPosition
{
	Top,
	Left,
	Right,
	Bottom
};

class WindowShadow
{
public:
	WindowShadow(HWND parentWindow, WindowShadowPosition position);
	~WindowShadow();

	void update();

private:
	void paintWindow();
	LRESULT onMessage(INT message, WPARAM wparam, LPARAM lparam);

	RECT getShadowRect();
	void updateWindowPos();

	static LRESULT CALLBACK wndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

	HWND handle = 0;
	HWND parentWindow = 0;
	WindowShadowPosition position;
	bool firstShow = true;

	WindowShadow(const WindowShadow &) = delete;
	WindowShadow &operator=(const WindowShadow &) = delete;
};

class DPIPixel
{
public:
	DPIPixel(HWND window)
	{
#if 1 // The official way this was done in Windows 7. Not sure if it works in all Windows 10 versions
		HDC dc = GetDC(window);
		int dpi = GetDeviceCaps(dc, LOGPIXELSX);
		ReleaseDC(window, dc);
#else // Requires Windows 10
		int dpi = GetDpiForWindow(window);
#endif
		scale = dpi / 96.0f;
	}

	float scale = 1.0f;

	int toScreen(int v) const { return (int)std::round(v * scale); }
	int fromScreen(int v) const { return (int)std::round(v / scale); }
};
