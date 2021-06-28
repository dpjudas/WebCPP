#pragma once

#include <string>
#include <memory>
#include <Windows.h>
#include <functional>
#include "win32_window_shadow.h"

class Win32Window
{
public:
	Win32Window(bool customFrame, bool popup);
	~Win32Window();

	void setPosition(int x, int y, int width, int height);

	int clientWidth() const;
	int clientHeight() const;

	float dpiScale() const;

	void setTitle(const std::string &name);
	void showWindow(bool maximized);
	void showCursor();
	void hideCursor();

	void setWindowRect(int x, int y, int width, int height);
	void setNeedsUpdate();
	void setFocus();

	std::function<void()> eventClose;
	std::function<void()> eventGeometryChanged;
	std::function<void(int x, int y)> eventLeftButtonDown;
	std::function<void(int x, int y)> eventLeftButtonDoubleClick;
	std::function<void(int x, int y)> eventLeftButtonUp;
	std::function<void(int x, int y)> eventRightButtonDown;
	std::function<void(int x, int y)> eventRightButtonDoubleClick;
	std::function<void(int x, int y)> eventRightButtonUp;
	std::function<void(int x, int y)> eventMouseMove;
	std::function<void()> eventMouseLeave;
	std::function<void(int x, int y)> eventRawMouseMove;
	std::function<void(int key)> eventKeyDown;
	std::function<void(int key)> eventKeyUp;
	std::function<void(std::string text)> eventKeyChar;
	std::function<int(int x, int y)> eventFrameHitTest;
	std::function<void()> eventPaint;

	HWND handle = 0;

	static void Quit();
	static void Exec();
	static void ProcessEvents();

private:
	LRESULT onMessage(INT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool customFrame = false;
	bool popup = false;
	std::unique_ptr<WindowShadow> shadowTop;
	std::unique_ptr<WindowShadow> shadowBottom;
	std::unique_ptr<WindowShadow> shadowLeft;
	std::unique_ptr<WindowShadow> shadowRight;

	bool hasFocus = false;
	bool cursorVisible = true;
	POINT lastCursorPos = {};

	bool trackingMouseLeave = false;

	Win32Window(const Win32Window &) = delete;
	Win32Window &operator=(const Win32Window &) = delete;

	friend class Win32WindowClass;
};
