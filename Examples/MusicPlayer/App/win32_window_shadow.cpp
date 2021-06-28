
#include "win32_window_shadow.h"
#include <stdexcept>
#include <vector>
#include <map>

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

WindowShadow::WindowShadow(HWND parentWindow, WindowShadowPosition position) : parentWindow(parentWindow), position(position)
{
	WNDCLASSEX classDesc = { 0 };
	classDesc.cbSize = sizeof(WNDCLASSEX);
	classDesc.hInstance = GetModuleHandle(0);
	classDesc.lpfnWndProc = WindowShadow::wndProc;
	classDesc.lpszClassName = L"WindowShadow";
	classDesc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	/*ATOM classHandle =*/ RegisterClassEx(&classDesc);

	DWORD exstyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE;
	DWORD style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	RECT box = getShadowRect();
	handle = CreateWindowEx(exstyle, L"WindowShadow", L"WindowShadow", style, box.left, box.top, box.right - box.left, box.bottom - box.top, parentWindow, 0, GetModuleHandle(0), this);
	if (handle == 0)
		throw std::runtime_error("CreateWindowEx failed");

	update();
}

WindowShadow::~WindowShadow()
{
	DestroyWindow(handle);
}

RECT WindowShadow::getShadowRect()
{
	RECT box = { 0 };
	GetWindowRect(parentWindow, &box);

	DPIPixel dpi(parentWindow);

	switch (position)
	{
	default:
	case WindowShadowPosition::Top:
		box.left -= dpi.toScreen(8);
		box.right += dpi.toScreen(8);
		box.top -= dpi.toScreen(8);
		box.bottom = box.top + dpi.toScreen(8);
		break;

	case WindowShadowPosition::Bottom:
		box.left -= dpi.toScreen(8);
		box.right += dpi.toScreen(8);
		box.top = box.bottom;
		box.bottom = box.top + dpi.toScreen(8);
		break;

	case WindowShadowPosition::Left:
		box.left -= dpi.toScreen(8);
		box.right = box.left + dpi.toScreen(8);
		break;

	case WindowShadowPosition::Right:
		box.left = box.right;
		box.right += dpi.toScreen(8);
		break;
	}

	return box;
}

void WindowShadow::updateWindowPos()
{
	bool isHidden = IsZoomed(parentWindow) || IsIconic(parentWindow) || !IsWindowVisible(parentWindow);

	UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
	if (isHidden)
		flags |= SWP_HIDEWINDOW;
	else
		flags |= SWP_SHOWWINDOW;

	RECT box = getShadowRect();
	SetWindowPos(handle, 0, box.left, box.top, box.right - box.left, box.bottom - box.top, flags);

	if (!isHidden && firstShow)
	{
		paintWindow();
		firstShow = false;
	}
}

void WindowShadow::update()
{
	if (firstShow)
	{
		SetTimer(handle, 1, 30, 0);
	}
	else
	{
		updateWindowPos();
	}
}

void WindowShadow::paintWindow()
{
	RECT box = { 0 };
	GetClientRect(handle, &box);

	DPIPixel dpi(parentWindow);

	uint32_t gradient[9] = { 167, 181, 200, 220, 235, 245, 250, 253, 254 };

	int width = box.right;
	int height = box.bottom;
	std::vector<uint32_t> pixelbuffer(width * height);
	uint32_t *pixel = pixelbuffer.data();
	for (int y = 0; y < height; y++)
	{
		uint32_t *line = pixel + y * width;
		for (int x = 0; x < width; x++)
		{
			float u = x / dpi.scale + 0.5f;
			float v = y / dpi.scale + 0.5f;
			float distance = 0.0f;

			float w = (float)width / dpi.scale;

			if (position == WindowShadowPosition::Left || position == WindowShadowPosition::Right)
			{
				std::swap(u, v);
				w = (float)height / dpi.scale + 16.0f;
				u += 8.0f;
			}

			if (position == WindowShadowPosition::Top || position == WindowShadowPosition::Right)
			{
				if (u < 14)
					distance = sqrt((14.0f - u) * (14.0f - u) + (v + 6.0f) * (v + 6.0f)) - 6.0f;
				else if (u > w - 14)
					distance = sqrt((float)(14.0 - w + u) * (14 - w + u) + (v + 6.0f) * (v + 6.0f)) - 6.0f;
				else
					distance = v;
			}
			else if (position == WindowShadowPosition::Bottom || position == WindowShadowPosition::Left)
			{
				if (u < 14)
					distance = sqrt((14.0f - u) * (14.0f - u) + (14.0f - v) * (14.0f - v)) - 6.0f;
				else if (u > w - 14)
					distance = sqrt((float)(14.0 - w + u) * (14 - w + u) + (14.0f - v) * (14.0f - v)) - 6.0f;
				else
					distance = 8.0f - v;
			}

			if (distance < 0.0f) distance = 0.0f;
			if (distance > 8.0f) distance = 8.0f;

			// Linear sample gradient array
			int index0 = (int)distance;
			int index1 = index0 != 8 ? index0 + 1 : index0;
			float t = distance - index0;
			uint32_t alpha = 255 - (int)(gradient[index0] * (1.0f - t) + gradient[index1] * t + 0.5f);

			line[x] = alpha << 24;
		}
	}

	BITMAPV5HEADER bmp_header;
	memset(&bmp_header, 0, sizeof(BITMAPV5HEADER));
	bmp_header.bV5Size = sizeof(BITMAPV5HEADER);
	bmp_header.bV5Width = width;
	bmp_header.bV5Height = height;
	bmp_header.bV5Planes = 1;
	bmp_header.bV5BitCount = 32;
	bmp_header.bV5Compression = BI_RGB;

	HDC hdc = GetDC(handle);
	HDC bitmap_dc = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateDIBitmap(hdc, (BITMAPINFOHEADER *)&bmp_header, CBM_INIT, pixelbuffer.data(), (BITMAPINFO *)&bmp_header, DIB_RGB_COLORS);
	HGDIOBJ old_bitmap = SelectObject(bitmap_dc, bitmap);

	SIZE size = { width, height };
	POINT point = { 0, 0 };
	COLORREF rgb_colorkey = RGB(0, 0, 0);
	BLENDFUNCTION blend;
	memset(&blend, 0, sizeof(BLENDFUNCTION));
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = (BYTE)255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(handle, 0, 0, &size, bitmap_dc, &point, rgb_colorkey, &blend, ULW_ALPHA);

	SelectObject(bitmap_dc, old_bitmap);
	DeleteObject(bitmap);
	DeleteDC(bitmap_dc);
	ReleaseDC(handle, hdc);
}

LRESULT WindowShadow::onMessage(INT message, WPARAM wparam, LPARAM lparam)
{
	static std::map<INT, INT> sizemoveMessages =
	{
		{ WM_MOUSEMOVE, WM_NCMOUSEMOVE },
		{ WM_LBUTTONDBLCLK, WM_NCLBUTTONDBLCLK },
		{ WM_LBUTTONDOWN, WM_NCLBUTTONDOWN },
		{ WM_LBUTTONUP, WM_NCLBUTTONUP },
		{ WM_MBUTTONDBLCLK, WM_NCMBUTTONDBLCLK },
		{ WM_MBUTTONDOWN, WM_NCMBUTTONDOWN },
		{ WM_MBUTTONUP, WM_NCMBUTTONUP },
		{ WM_RBUTTONDBLCLK, WM_NCRBUTTONDBLCLK },
		{ WM_RBUTTONDOWN, WM_NCRBUTTONDOWN },
		{ WM_RBUTTONUP, WM_NCRBUTTONUP },
		{ WM_XBUTTONDBLCLK, WM_NCXBUTTONDBLCLK },
		{ WM_XBUTTONDOWN, WM_NCXBUTTONDOWN },
		{ WM_XBUTTONUP, WM_NCXBUTTONUP }
	};

	if (message == WM_LBUTTONDOWN)
		SetForegroundWindow(parentWindow);

	auto it = sizemoveMessages.find(message);
	if (it != sizemoveMessages.end())
	{
		WPARAM hittest = HTNOWHERE;
		int x = (int16_t)LOWORD(lparam);
		int y = (int16_t)HIWORD(lparam);

		RECT box = { 0 };
		GetClientRect(handle, &box);
		int width = box.right - box.left;
		int height = box.bottom - box.top;

		DPIPixel dpi(parentWindow);

		auto style = GetWindowLongPtr(parentWindow, GWL_STYLE);
		if (!!(style & WS_THICKFRAME))
		{
			if (position == WindowShadowPosition::Left)
			{
				if (y < dpi.toScreen(8))
				{
					hittest = HTTOPLEFT;
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
				}
				else if (y >= height - dpi.toScreen(8))
				{
					hittest = HTBOTTOMLEFT;
					SetCursor(LoadCursor(0, IDC_SIZENESW));
				}
				else
				{
					hittest = HTLEFT;
					SetCursor(LoadCursor(0, IDC_SIZEWE));
				}
			}
			else if (position == WindowShadowPosition::Right)
			{
				if (y < dpi.toScreen(8))
				{
					hittest = HTTOPRIGHT;
					SetCursor(LoadCursor(0, IDC_SIZENESW));
				}
				else if (y >= height - dpi.toScreen(8))
				{
					hittest = HTBOTTOMRIGHT;
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
				}
				else
				{
					hittest = HTRIGHT;
					SetCursor(LoadCursor(0, IDC_SIZEWE));
				}
			}
			else if (position == WindowShadowPosition::Top)
			{
				if (x < dpi.toScreen(16))
				{
					hittest = HTTOPLEFT;
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
				}
				else if (x >= width - dpi.toScreen(16))
				{
					hittest = HTTOPRIGHT;
					SetCursor(LoadCursor(0, IDC_SIZENESW));
				}
				else
				{
					hittest = HTTOP;
					SetCursor(LoadCursor(0, IDC_SIZENS));
				}
			}
			else if (position == WindowShadowPosition::Bottom)
			{
				if (x < dpi.toScreen(16))
				{
					hittest = HTBOTTOMLEFT;
					SetCursor(LoadCursor(0, IDC_SIZENESW));
				}
				else if (x >= width - dpi.toScreen(16))
				{
					hittest = HTBOTTOMRIGHT;
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
				}
				else
				{
					hittest = HTBOTTOM;
					SetCursor(LoadCursor(0, IDC_SIZENS));
				}
			}
		}
		else
		{
			hittest = (WPARAM)HTTRANSPARENT;
			SetCursor(LoadCursor(0, IDC_ARROW));
		}

		return SendMessage(parentWindow, it->second, hittest, lparam);
	}
	else if (message == WM_MOUSEHOVER)
	{
		return SendMessage(parentWindow, WM_NCMOUSEHOVER, 0, 0);
	}
	else if (message == WM_MOUSELEAVE)
	{
		return SendMessage(parentWindow, WM_NCMOUSELEAVE, 0, 0);
	}
	else if (message == WM_SIZE)
	{
		paintWindow();
	}
	else if (message == WM_DPICHANGED)
	{
		update();
		return 0;
	}
	else if (message == WM_TIMER && wparam == 1)
	{
		KillTimer(handle, 1);
		updateWindowPos();
		paintWindow();
		return 0;
	}

	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT WindowShadow::wndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE)
	{
		CREATESTRUCT *createInfo = (CREATESTRUCT*)lparam;
		auto window = reinterpret_cast<WindowShadow*>(createInfo->lpCreateParams);
		window->handle = handle;
		SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(window));
		return window->onMessage(message, wparam, lparam);
	}
	else
	{
		auto window = reinterpret_cast<WindowShadow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
		if (window)
			return window->onMessage(message, wparam, lparam);
		else
			return DefWindowProc(handle, message, wparam, lparam);
	}
}
