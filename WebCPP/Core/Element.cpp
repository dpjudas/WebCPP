
#include "Element.h"
#include "Event.h"
#include "JSCallback.h"

Element::Element(JSValue handle) : handle(std::move(handle))
{
}

Element::Element(std::string elementType) : handle(JSValue::global("document").call<JSValue>("createElement", elementType))
{
}

Element::Element(std::string ns, std::string elementType) : handle(JSValue::global("document").call<JSValue>("createElementNS", ns, elementType))
{
}

Element::~Element()
{
}

void Element::insertBefore(Element* newElement, Element* insertPoint)
{
	handle.call<void>("insertBefore", newElement->handle, insertPoint->handle);
}

void Element::appendChild(Element* newElement)
{
	handle.call<void>("appendChild", newElement->handle);
}

void Element::removeChild(Element* childElement)
{
	handle.call<void>("removeChild", childElement->handle);
}

std::string Element::getAttribute(std::string name) const
{
	return handle.call<JSValue>("getAttribute", name).as<std::string>();
}

void Element::setAttribute(std::string name, std::string value)
{
	handle.call<void>("setAttribute", name, value);
}

std::string Element::getAttribute(std::string name)
{
	JSValue value = handle.call<JSValue>("getAttribute", name);
	if (value.isNull())
		return {};
	return value.as<std::string>();
}

void Element::removeAttribute(std::string name)
{
	handle.call<void>("removeAttribute", name);
}

std::string Element::attributeFromPoint(std::string name, double clientX, double clientY, const std::string& defaultValue)
{
	JSValue element = JSValue::global("document").call<JSValue>("elementFromPoint", clientX, clientY);
	while (!element.isNull())
	{
		JSValue value = element.call<JSValue>("getAttribute", name);
		if (!value.isNull())
		{
			return value.as<std::string>();
		}
		element = element["parentElement"];
	}
	return defaultValue;
}

std::string Element::getStyle(std::string name) const
{
	if (handle["style"][name].isString() == true)
		return handle["style"][name].as<std::string>();
	else
		return std::string();
}

void Element::setStyle(std::string name, std::string value)
{
	handle["style"].set(name, value);
}

void Element::removeStyle(std::string name)
{
	handle["style"].call<void>("removeProperty", name);
}

void Element::click()
{
	handle.call<void>("click");
}

void Element::focus()
{
	handle.call<void>("focus");
}

void Element::dispatchEvent(std::string name, bool bubbles)
{
	auto options = JSValue::object();
	options.set("bubbles", bubbles);
	handle.call<void>("dispatchEvent", JSValue::global("Event").new_(name, options));
}

void Element::addEventListener(std::string name, std::function<void(Event* event)> handler)
{
	auto wrap = [=](JSValue args) -> JSValue
	{
		Event event(args[0]);
		handler(&event);
		return JSValue::undefined();
	};

	auto callback = std::make_unique<JSCallback>(wrap);
	handle.call<void>("addEventListener", name, callback->getHandler());
	eventListeners.push_back(std::move(callback));
}

void Element::setPointerCapture(JSValue id)
{
	handle.call<void>("setPointerCapture", id);
}

void Element::releasePointerCapture(JSValue id)
{
	handle.call<void>("releasePointerCapture", id);
}

Rect Element::getBoundingClientRect()
{
	JSValue result = handle.call<JSValue>("getBoundingClientRect");

	Rect box;
	box.x = result["left"].as<double>();
	box.y = result["top"].as<double>();
	box.width = result["width"].as<double>();
	box.height = result["height"].as<double>();
	return box;
}

double Element::clientTop() const
{
	return handle["clientTop"].as<double>();
}

double Element::clientLeft() const
{
	return handle["clientLeft"].as<double>();
}

double Element::clientWidth() const
{
	return handle["clientWidth"].as<double>();
}

double Element::clientHeight() const
{
	return handle["clientHeight"].as<double>();
}

double Element::offsetLeft() const
{
	return handle["offsetLeft"].as<double>();
}

double Element::offsetTop() const
{
	return handle["offsetTop"].as<double>();
}

double Element::offsetWidth() const
{
	return handle["offsetWidth"].as<double>();
}

double Element::offsetHeight() const
{
	return handle["offsetHeight"].as<double>();
}

double Element::scrollLeft() const
{
	return handle["scrollLeft"].as<double>();
}

double Element::scrollTop() const
{
	return handle["scrollTop"].as<double>();
}

double Element::scrollWidth() const
{
	return handle["scrollWidth"].as<double>();
}

double Element::scrollHeight() const
{
	return handle["scrollHeight"].as<double>();
}

void Element::scrollTo(double x, double y)
{
	handle.call<void>("scrollTo", x, y);
}

void Element::scrollBy(double x, double y)
{
	handle.call<void>("scrollBy", x, y);
}

void Element::setScrollLeft(double x)
{
	handle.set("scrollLeft", x);
}

void Element::setScrollTop(double y)
{
	handle.set("scrollTop", y);
}

void Element::setInnerText(const std::string& text)
{
	handle.set("innerText", text);
}

void Element::setInnerHtml(const std::string& html)
{
	handle.set("innerHTML", html);
}
