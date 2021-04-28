#pragma once

#include <functional>
#include <vector>
#include "Event.h"
#include "JSValue.h"

class JSCallback;

class Rect
{
public:
	Rect() = default;
	Rect(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) { }

	static Rect ltrb(double left, double top, double right, double bottom) { return Rect(left, top, right - left, bottom - top); }

	double x = 0;
	double y = 0;
	double width = 0;
	double height = 0;
};

class Element
{
public:
	Element(JSValue handle);
	~Element();

	void setAttribute(std::string name, std::string value);
	void removeAttribute(std::string name);

	void setStyle(std::string name, std::string value);

	void addEventListener(std::string name, std::function<void(Event* event)> handler);

	Rect getBoundingClientRect();

	JSValue handle;

private:
	std::vector<std::unique_ptr<JSCallback>> eventListeners;
};
