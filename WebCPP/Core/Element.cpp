
#include "Element.h"
#include "Event.h"
#include "JSCallback.h"

Element::Element(JSValue handle) : handle(std::move(handle))
{
}

Element::~Element()
{
}

void Element::setAttribute(std::string name, std::string value)
{
	handle.call<void>("setAttribute", name, value);
}

void Element::removeAttribute(std::string name)
{
	handle.call<void>("removeAttribute", name);
}

void Element::setStyle(std::string name, std::string value)
{
	handle["style"].set(name, value);
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
