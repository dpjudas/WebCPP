
#include "win32_window.h"
#include "resource.h"
#include <windowsx.h>
#include <vector>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC		((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE	((USHORT) 0x02)
#endif

#ifndef HID_USAGE_GENERIC_JOYSTICK
#define HID_USAGE_GENERIC_JOYSTICK	((USHORT) 0x04)
#endif

#ifndef HID_USAGE_GENERIC_GAMEPAD
#define HID_USAGE_GENERIC_GAMEPAD	((USHORT) 0x05)
#endif

#ifndef RIDEV_INPUTSINK
#define RIDEV_INPUTSINK	(0x100)
#endif

class Win32WindowClass
{
public:
	Win32WindowClass()
	{
		WNDCLASSEX windowClassDesc;
		memset(&windowClassDesc, 0, sizeof(WNDCLASSEX));
		windowClassDesc.cbSize = sizeof(WNDCLASSEX);
		windowClassDesc.lpszClassName = ClassName;
		windowClassDesc.hInstance = GetModuleHandle(nullptr);
		windowClassDesc.lpfnWndProc = &Win32Window::WindowProc;
		windowClassDesc.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClassDesc.style |= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		windowClassDesc.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CEFSIMPLE));
		windowClassDesc.hIconSm = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SMALL));
		RegisterClassEx(&windowClassDesc);

		windowClassDesc.lpszClassName = ClassNamePopup;
		windowClassDesc.style |= CS_DROPSHADOW;
		RegisterClassEx(&windowClassDesc);
	}

	const wchar_t *ClassName = L"Win32Window";
	const wchar_t *ClassNamePopup = L"Win32WindowPopup";
};

Win32Window::Win32Window(bool customFrame, bool popup) : customFrame(customFrame), popup(popup)
{
	HDC screendc = GetDC(0);
	int screenwidth = GetDeviceCaps(screendc, HORZRES);
	int screenheight = GetDeviceCaps(screendc, VERTRES);
	ReleaseDC(0, screendc);
	int width = screenwidth * 80 / 100;
	int height = screenheight * 80 / 100;
	int x = (screenwidth - width) / 2;
	int y = (screenheight - height) / 2;

	DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD exstyle = 0;

	if (!popup)
	{
		style |= WS_OVERLAPPEDWINDOW;
		exstyle |= WS_EX_APPWINDOW;
	}
	else
	{
		style |= WS_POPUP;
		exstyle |= WS_EX_TOOLWINDOW;
	}

	static Win32WindowClass win32class;
	handle = CreateWindowEx(exstyle, popup ? win32class.ClassNamePopup : win32class.ClassName, L"Music Player", style, x, y, width, height, 0, 0, GetModuleHandle(nullptr), this);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid.usUsage = HID_USAGE_GENERIC_MOUSE;
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = handle;
	RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
}

Win32Window::~Win32Window()
{
	if (handle)
		DestroyWindow(handle);
}

void Win32Window::setPosition(int x, int y, int width, int height)
{
	SetWindowPos(handle, 0, x, y, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void Win32Window::setTitle(const std::string &name)
{
	SetWindowTextA(handle, name.c_str());
}

void Win32Window::showWindow(bool maximized)
{
	ShowWindow(handle, maximized ? SW_SHOWMAXIMIZED : SW_SHOW);
}

void Win32Window::setFocus()
{
	SetFocus(handle);
}

void Win32Window::showCursor()
{
	if (!cursorVisible && hasFocus)
	{
		SetCursorPos(lastCursorPos.x, lastCursorPos.y);
		ShowCursor(true);
	}
	cursorVisible = true;
}

void Win32Window::hideCursor()
{
	if (cursorVisible && hasFocus)
	{
		ShowCursor(false);
		RECT box;
		GetCursorPos(&lastCursorPos);
		if (GetWindowRect(handle, &box))
			SetCursorPos((box.left + box.right) / 2, (box.top + box.bottom) / 2);
	}
	cursorVisible = false;
}

int Win32Window::clientWidth() const
{
	RECT box = { };
	GetClientRect(handle, &box);
	return box.right - box.left;
}

int Win32Window::clientHeight() const
{
	RECT box = { };
	GetClientRect(handle, &box);
	return box.bottom - box.top;
}

float Win32Window::dpiScale() const
{
	HDC dc = GetDC(handle);
	float dpiScale = GetDeviceCaps(dc, LOGPIXELSX) / 96.0f;
	ReleaseDC(handle, dc);
	return dpiScale;
}

void Win32Window::setWindowRect(int x, int y, int width, int height)
{
	SetWindowPos(handle, 0, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

void Win32Window::setNeedsUpdate()
{
	InvalidateRect(handle, nullptr, FALSE);
}

LRESULT Win32Window::onMessage(INT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE && customFrame)
	{
		shadowTop = std::make_unique<WindowShadow>(handle, WindowShadowPosition::Top);
		shadowBottom = std::make_unique<WindowShadow>(handle, WindowShadowPosition::Bottom);
		shadowLeft = std::make_unique<WindowShadow>(handle, WindowShadowPosition::Left);
		shadowRight = std::make_unique<WindowShadow>(handle, WindowShadowPosition::Right);

		SetWindowPos(handle, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	}
	else if (message == WM_DESTROY)
	{
		handle = 0;
	}
	else if (message == WM_NCCALCSIZE && customFrame)
	{
		return 0;
	}
	else if (message == WM_NCHITTEST && customFrame)
	{
		if (eventFrameHitTest)
		{
			POINT pt = { (int16_t)LOWORD(lparam), (int16_t)HIWORD(lparam) };
			ScreenToClient(handle, &pt);
			return eventFrameHitTest(pt.x, pt.y);
		}
		else
		{
			return HTCLIENT;
		}
	}
	else if (message == WM_CLOSE)
	{
		if (eventClose)
			eventClose();
	}
	else if (message == WM_ERASEBKGND)
	{
		return 0;
	}
	else if (message == WM_PAINT)
	{
		if (eventPaint)
			eventPaint();
#if 0
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(handle, &ps);
		RECT box;
		GetClientRect(handle, &box);
		HBRUSH yellow = CreateSolidBrush(RGB(255, 255, 0));
		FillRect(dc, &box, yellow);
		DeleteObject(yellow);
		EndPaint(handle, &ps);
		return 0;
#endif
	}
	else if (message == WM_WINDOWPOSCHANGED)
	{
		if (shadowTop)
		{
			shadowTop->update();
			shadowBottom->update();
			shadowLeft->update();
			shadowRight->update();
		}

		if (eventGeometryChanged)
			eventGeometryChanged();
	}
	else if (message == WM_SETFOCUS)
	{
		hasFocus = true;
		if (!cursorVisible)
		{
			ShowCursor(false);
			RECT box;
			GetCursorPos(&lastCursorPos);
			if (GetWindowRect(handle, &box))
				SetCursorPos((box.left + box.right) / 2, (box.top + box.bottom) / 2);
		}
	}
	else if (message == WM_KILLFOCUS)
	{
		hasFocus = false;
		if (!cursorVisible)
		{
			SetCursorPos(lastCursorPos.x, lastCursorPos.y);
			ShowCursor(true);
		}
	}
	else if (message == WM_KEYDOWN)
	{
		int vkey = (int)wparam;
		if ((lparam >> 16) == 0x29) // keycode for button left of 1
			vkey = 0xff;

		if (eventKeyDown)
			eventKeyDown(vkey);
		return 0;
	}
	else if (message == WM_KEYUP)
	{
		if (eventKeyUp)
			eventKeyUp((int)wparam);
		return 0;
	}
	else if (message == WM_CHAR)
	{
		//if (eventKeyChar)
		//	eventKeyChar(Text::from_utf32((unsigned int)wparam));
		return 0;
	}
	else if (message == WM_INPUT)
	{
		HRAWINPUT rawinputHandle = (HRAWINPUT)lparam;
		UINT size = 0;
		UINT result = GetRawInputData(rawinputHandle, RID_INPUT, 0, &size, sizeof(RAWINPUTHEADER));
		if (result == 0 && size > 0)
		{
			std::vector<uint32_t> buf((size + 3) / 4);
			result = GetRawInputData(rawinputHandle, RID_INPUT, buf.data(), &size, sizeof(RAWINPUTHEADER));
			if (result >= 0)
			{
				RAWINPUT *rawinput = (RAWINPUT*)buf.data();
				if (rawinput->header.dwType == RIM_TYPEMOUSE)
				{
					if (!cursorVisible && GetFocus() == handle)
					{
						if (eventRawMouseMove)
							eventRawMouseMove(rawinput->data.mouse.lLastX, rawinput->data.mouse.lLastY);
						RECT box;
						if (GetWindowRect(handle, &box))
							SetCursorPos((box.left + box.right) / 2, (box.top + box.bottom) / 2);
					}
				}
			}
		}
	}
	else if (message == WM_LBUTTONDOWN)
	{
		if (eventLeftButtonDown)
			eventLeftButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_LBUTTONDBLCLK)
	{
		if (eventLeftButtonDoubleClick)
			eventLeftButtonDoubleClick(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_LBUTTONUP)
	{
		if (eventLeftButtonUp)
			eventLeftButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_RBUTTONDOWN)
	{
		if (eventRightButtonDown)
			eventRightButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_RBUTTONDBLCLK)
	{
		if (eventRightButtonDoubleClick)
			eventRightButtonDoubleClick(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_RBUTTONUP)
	{
		if (eventRightButtonUp)
			eventRightButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}
	else if (message == WM_MOUSEMOVE)
	{
		if (eventMouseMove)
			eventMouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));

		if (!trackingMouseLeave)
		{
			TRACKMOUSEEVENT eventTrack = {};
			eventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
			eventTrack.dwFlags = TME_LEAVE;
			eventTrack.dwHoverTime = HOVER_DEFAULT;
			eventTrack.hwndTrack = handle;
			trackingMouseLeave = TrackMouseEvent(&eventTrack) == TRUE;
		}
		return 0;
	}
	else if (message == WM_MOUSELEAVE)
	{
		trackingMouseLeave = false;
		if (eventMouseLeave)
			eventMouseLeave();
		return 0;
	}
	return DefWindowProc(handle, message, wparam, lparam);
}

LRESULT Win32Window::WindowProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE)
	{
		CREATESTRUCT *createInfo = (CREATESTRUCT*)lparam;
		auto window = reinterpret_cast<Win32Window*>(createInfo->lpCreateParams);
		window->handle = handle;
		SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(window));
		return window->onMessage(message, wparam, lparam);
	}
	else
	{
		auto window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(handle, GWLP_USERDATA));
		if (window)
			return window->onMessage(message, wparam, lparam);
		else
			return DefWindowProc(handle, message, wparam, lparam);
	}
}

void Win32Window::Quit()
{
	PostQuitMessage(0);
}

void Win32Window::Exec()
{
	while (true)
	{
		MSG msg;
		BOOL result = GetMessage(&msg, 0, 0, 0);
		if (result <= 0 || msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Win32Window::ProcessEvents()
{
	while (true)
	{
		MSG msg;
		BOOL result = PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
		if (result == 0 || msg.message == WM_QUIT)
			break;

		if (msg.message == WM_KEYDOWN && (msg.lParam >> 16) != 0x29) // Don't generate keychar events for the key left of 1
			TranslateMessage(&msg);

		DispatchMessage(&msg);
	}
}
