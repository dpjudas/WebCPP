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
	Element(std::string elementType);
	Element(std::string ns, std::string elementType);
	~Element();

	void insertBefore(Element* newElement, Element* insertPoint);
	void appendChild(Element* newElement);
	void removeChild(Element* childElement);

	std::string getAttribute(std::string name) const;
	void setAttribute(std::string name, std::string value);
	void removeAttribute(std::string name);
	std::string getAttribute(std::string name);

	static std::string attributeFromPoint(std::string name, double clientX, double clientY, const std::string& defaultValue = {});

	std::string getStyle(std::string name) const;
	void setStyle(std::string name, std::string value);
	void removeStyle(std::string name);

	void addEventListener(std::string name, std::function<void(Event* event)> handler);
	void dispatchEvent(std::string name, bool bubbles = true);

	void setPointerCapture(JSValue id);
	void releasePointerCapture(JSValue id);

	void setInnerText(const std::string& text);
	void setInnerHtml(const std::string& html);

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
