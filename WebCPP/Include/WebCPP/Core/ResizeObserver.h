#pragma once

#include "JSValue.h"
#include <functional>

namespace web
{
	class Element;
	class JSCallback;

	class ResizeBoxSize
	{
	public:
		double inlineSize = 0.0;
		double blockSize = 0.0;
	};

	class ResizeObserverEntry
	{
	public:
		ResizeObserverEntry(JSValue entry);

		std::vector<ResizeBoxSize> getBorderBoxSize();
		std::vector<ResizeBoxSize> getContentBoxSize();

	private:
		JSValue entry;
	};

	class ResizeObserver
	{
	public:
		ResizeObserver();
		~ResizeObserver();

		void observe(Element* element);
		void unobserve(Element* element);

		std::function<void(std::vector<ResizeObserverEntry> entries)> onResize;

	private:
		ResizeObserver(const ResizeObserver&) = delete;
		ResizeObserver& operator=(const ResizeObserver&) = delete;

		JSValue handle = JSValue::undefined();
		std::unique_ptr<JSCallback> callback;
		std::vector<Element*> observers;
	};
}
