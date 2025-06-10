#include "EventListener.h"
#include "WebCPP/Core/Element.h"
#include "WebCPP/Core/Event.h"


EventListener::EventListener(std::string name, std::function<void(Event*)> handler)
	: name(name), handler(handler)
{
}

EventListener::EventListener(std::string name, std::function<void(Event*)> handler, Element* element)
	: name(name), handler(handler)
{
	addEventListener(element);
}

EventListener::~EventListener()
{
	removeEventListener();
}

void EventListener::addEventListener(Element* element)
{
	if (listener)
		removeEventListener();

	listener = element;

	auto wrap = [=](JSValue args) -> JSValue
		{
			Event event(args[0]);
			handler(&event);
			return JSValue::undefined();
		};

	callback = std::make_unique<JSCallback>(wrap);
	listener->handle.call<void>("addEventListener", name, callback->getHandler());
}

void EventListener::removeEventListener()
{
	if (!listener)
		return;

	listener->handle.call<void>("removeEventListener", name, callback->getHandler());
	listener = nullptr;
	callback.reset();
}
