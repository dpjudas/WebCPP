
#include "WebCPP/Core/Event.h"

Event::Event(JSValue handle) : handle(std::move(handle))
{
}

JSValue Event::getPointerId()
{
	return handle["pointerId"];
}

int Event::getKeyCode()
{
	return handle["keyCode"].as<int>();
}

void Event::stopPropagation()
{
	handle.call<void>("stopPropagation");
}

void Event::stopImmediatePropagation()
{
	handle.call<void>("stopImmediatePropagation");
}

void Event::preventDefault()
{
	handle.call<void>("preventDefault");
}

JSValue Event::pointerId() const
{
	return handle["pointerId"];
}

JSValue Event::target() const
{
	return handle["target"];
}

double Event::clientX()
{
	return handle["clientX"].as<double>();
}

double Event::clientY()
{
	return handle["clientY"].as<double>();
}

double Event::offsetX()
{
	return handle["offsetX"].as<double>();
}

double Event::offsetY()
{
	return handle["offsetY"].as<double>();
}
