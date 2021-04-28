#pragma once

#include "JSValue.h"

class Event
{
public:
	Event(JSValue handle);

	void stopPropagation();
	void stopImmediatePropagation();
	void preventDefault();

	JSValue handle;
};
