#pragma once

#include "JSValue.h"

namespace web
{
	class Event
	{
	public:
		Event(JSValue handle);

		double clientX();
		double clientY();
		double offsetX();
		double offsetY();
		JSValue getPointerId();

		int getKeyCode();

		void stopPropagation();
		void stopImmediatePropagation();
		void preventDefault();

		JSValue pointerId() const;

		JSValue target() const;

		JSValue handle;
	};
}
