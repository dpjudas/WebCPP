
#include "WebCPP/Core/Timer.h"
#include "WebCPP/Core/JSCallback.h"
#include <map>

namespace
{
	std::map<int, std::unique_ptr<JSCallback>> pendingTimeouts;
	std::map<int, std::unique_ptr<JSCallback>> pendingIntervals;
}

int setTimeout(std::function<void()> onTimeout, int delay)
{
	auto ontimeout = std::make_unique<JSCallback>();
	int id = JSValue::global("window").call<int>("setTimeout", ontimeout->getHandler(), delay);
	ontimeout->setCallback([=](JSValue args) -> JSValue
		{
			onTimeout();
			pendingTimeouts.erase(id);
			return JSValue::undefined();
		});

	pendingTimeouts.insert({ id, std::move(ontimeout) });
	return id;
}

void clearTimeout(int timeoutID)
{
	JSValue::global("window").call<void>("clearTimeout", timeoutID);
	pendingTimeouts.erase(timeoutID);
}

int setInterval(std::function<void()> onInterval, int delay)
{
	auto oninterval = std::make_unique<JSCallback>();
	int id = JSValue::global("window").call<int>("setInterval", oninterval->getHandler(), delay);
	oninterval->setCallback([=](JSValue args) -> JSValue
	{
		onInterval();
		pendingIntervals.erase(id);
		return JSValue::undefined();
	});

	pendingIntervals.insert({ id, std::move(oninterval) });
	return id;
}

void clearInterval(int intervalID)
{
	JSValue::global("window").call<void>("clearInterval", intervalID);
	pendingIntervals.erase(intervalID);
}
