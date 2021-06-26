#pragma once

#include <functional>
#include <vector>
#include "Event.h"
#include "JSValue.h"

class JSCallback;

class Point
{
public:
	Point() = default;
	Point(double x, double y) : x(x), y(y) { }

	double x = 0;
	double y = 0;
};

class Size
{
public:
	Size() = default;
	Size(double width, double height) : width(width), height(height) { }

	double width = 0;
	double height = 0;
};

class Rect
{
public:
	Rect() = default;
	Rect(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) { }

	Point pos() const { return { x, y }; }
	Size size() const { return { width, height }; }

	Point topLeft() const { return { x, y }; }
	Point topRight() const { return { x + width, y }; }
	Point bottomLeft() const { return { x, y + height }; }
	Point bottomRight() const { return { x + width, y + height }; }

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
	void dispatchEvent(std::string name, bool bubbles = true);

	void click();
	void focus();
	void setTabIndex(int index) { setAttribute("tabindex", std::to_string(index)); }
	void removeTabIndex() { removeAttribute("tabindex"); }

	Rect getBoundingClientRect();

	double clientTop() const;
	double clientLeft() const;
	double clientWidth() const;
	double clientHeight() const;

	double offsetLeft() const;
	double offsetTop() const;
	double offsetWidth() const;
	double offsetHeight() const;

	double scrollLeft() const;
	double scrollTop() const;
	double scrollWidth() const;
	double scrollHeight() const;

	void scrollTo(double x, double y);
	void scrollBy(double x, double y);
	void setScrollLeft(double x);
	void setScrollTop(double y);

	JSValue handle;

private:
	std::vector<std::unique_ptr<JSCallback>> eventListeners;
};
