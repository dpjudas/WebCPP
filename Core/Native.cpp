
#include "Native.h"
#include "JSCallback.h"

Native* Native::get()
{
	static Native native;
	return &native;
}

void Native::query(JSValue request, std::function<void(JSValue response)> onSuccess, std::function<void(int code, JSValue message)> onFailure)
{
	auto successHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		JSValue response = JSValue::global("JSON").call<JSValue>("parse", args[0]);
		if (onSuccess)
			onSuccess(response);
		return JSValue::undefined();
	});

	auto failureHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		int code = args[0].as<int>();
		JSValue response = (code != -1) ? JSValue::global("JSON").call<JSValue>("parse", args[1]) : args[1];
		if (onFailure)
			onFailure(code, response);
		return JSValue::undefined();
	});

	JSValue args = JSValue::object();
	args.set("request", JSValue::global("JSON").call<JSValue>("stringify", request));
	args.set("persistent", false);
	args.set("onSuccess", successHandler->getHandler());
	args.set("onFailure", failureHandler->getHandler());

	pendingRequests.push_back(std::move(successHandler));
	pendingRequests.push_back(std::move(failureHandler));

	JSValue::global("window").call<void>("cefQuery", args);
}

int Native::subscribe(JSValue request, std::function<void(JSValue message)> onMessage, std::function<void(int code, JSValue message)> onUnsubscribe)
{
	auto successHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		JSValue response = JSValue::global("JSON").call<JSValue>("parse", args[0]);
		if (onMessage)
			onMessage(response);
		return JSValue::undefined();
	});

	auto failureHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		int code = args[0].as<int>();
		JSValue response = (code != -1) ? JSValue::global("JSON").call<JSValue>("parse", args[1]) : args[1];
		if (onUnsubscribe)
			onUnsubscribe(code, response);
		return JSValue::undefined();
	});

	JSValue args = JSValue::object();
	args.set("request", JSValue::global("JSON").call<JSValue>("stringify", request));
	args.set("persistent", true);
	args.set("onSuccess", successHandler->getHandler());
	args.set("onFailure", failureHandler->getHandler());

	pendingRequests.push_back(std::move(successHandler));
	pendingRequests.push_back(std::move(failureHandler));

	return JSValue::global("window").call<int>("cefQuery", args);
}

void Native::unsubscribe(int id)
{
	JSValue::global("window").call<void>("cefQueryCancel", id);
}
