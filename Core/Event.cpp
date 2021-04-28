
#include "Event.h"

Event::Event(JSValue handle) : handle(std::move(handle))
{
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
