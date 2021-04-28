
#include "Timer.h"
#include "JSCallback.h"

namespace
{
	std::vector<std::unique_ptr<JSCallback>> pendingTimeouts;
	std::vector<std::unique_ptr<JSCallback>> pendingIntervals;
}

int setTimeout(std::function<void()> onTimeout, int delay)
{
	auto ontimeout = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		onTimeout();
		return JSValue::undefined();
	});

	int id = JSValue::global("window").call<int>("setTimeout", ontimeout->getHandler(), delay);
	pendingTimeouts.push_back(std::move(ontimeout));
	return id;
}

void clearTimeout(int timeoutID)
{
	JSValue::global("window").call<void>("clearTimeout", timeoutID);
}

int setInterval(std::function<void()> onInterval, int delay)
{
	auto oninterval = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		onInterval();
		return JSValue::undefined();
	});

	int id = JSValue::global("window").call<int>("setInterval", oninterval->getHandler(), delay);
	pendingIntervals.push_back(std::move(oninterval));
	return id;
}

void clearInterval(int intervalID)
{
	JSValue::global("window").call<void>("clearInterval", intervalID);
}
