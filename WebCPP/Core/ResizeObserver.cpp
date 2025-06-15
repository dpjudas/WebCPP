
#include "WebCPP/Core/ResizeObserver.h"
#include "WebCPP/Core/JSCallback.h"
#include "WebCPP/Core/Element.h"

namespace web
{
	ResizeObserver::ResizeObserver()
	{
		auto wrap = [this](JSValue args) -> JSValue
			{
				if (onResize)
				{
					std::vector<ResizeObserverEntry> entries;
					for (JSValue value : vecFromJSArray<JSValue>(args[0]))
					{
						entries.emplace_back(std::move(value));
					}
					onResize(std::move(entries));
				}
				return JSValue::undefined();
			};

		callback = std::make_unique<JSCallback>(wrap);
		handle = JSValue::global("ResizeObserver").new_(callback->getHandler());
	}

	ResizeObserver::~ResizeObserver()
	{
		for (Element* element : observers)
		{
			handle.call<void>("unobserve", element->handle);
		}
	}

	void ResizeObserver::observe(Element* element)
	{
		auto it = std::find(observers.begin(), observers.end(), element);
		if (it == observers.end())
		{
			observers.push_back(element);
			handle.call<void>("observe", element->handle);
		}
	}

	void ResizeObserver::unobserve(Element* element)
	{
		auto it = std::find(observers.begin(), observers.end(), element);
		if (it != observers.end())
		{
			handle.call<void>("unobserve", (*it)->handle);
			observers.erase(it);
		}
	}

	/////////////////////////////////////////////////////////////////////////////

	ResizeObserverEntry::ResizeObserverEntry(JSValue entry) : entry(std::move(entry))
	{
	}

	std::vector<ResizeBoxSize> ResizeObserverEntry::getBorderBoxSize()
	{
		std::vector<ResizeBoxSize> sizes;
		for (JSValue value : vecFromJSArray<JSValue>(entry["borderBoxSize"]))
		{
			ResizeBoxSize boxsize;
			boxsize.blockSize = value["blockSize"].as<double>();
			boxsize.inlineSize = value["inlineSize"].as<double>();
			sizes.push_back(boxsize);
		}
		return sizes;
	}

	std::vector<ResizeBoxSize> ResizeObserverEntry::getContentBoxSize()
	{
		std::vector<ResizeBoxSize> sizes;
		for (JSValue value : vecFromJSArray<JSValue>(entry["contentBoxSize"]))
		{
			ResizeBoxSize boxsize;
			boxsize.blockSize = value["blockSize"].as<double>();
			boxsize.inlineSize = value["inlineSize"].as<double>();
			sizes.push_back(boxsize);
		}
		return sizes;
	}
}
