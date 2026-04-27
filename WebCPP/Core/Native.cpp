
#include "WebCPP/Core/Native.h"
#include "WebCPP/Core/JSCallback.h"

namespace web
{
	namespace
	{
		std::vector<std::unique_ptr<JSCallback>> pendingRequests;
	}

	bool Native::isApp()
	{
		return !JSValue::global("window")["cefQuery"].isUndefined();
	}

	task<JsonValue> Native::query(const JsonValue& request)
	{
		auto promise = std::make_shared<web::task_promise<web::JsonValue>>();

		auto successHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				auto responseJson = web::JsonValue::parse(args[0].as<std::string>());
				promise->set_value(responseJson);
				return JSValue::undefined();
			});

		auto failureHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				try
				{
					int code = args[0].as<int>();
					std::string message = args[1].as<std::string>();
					throw NativeException(code, message);
				}
				catch (...)
				{
					promise->set_exception(std::current_exception());
				}
				return JSValue::undefined();
			});

		JSValue args = JSValue::object();
		args.set("request", request.to_json());
		args.set("persistent", false);
		args.set("onSuccess", successHandler->getHandler());
		args.set("onFailure", failureHandler->getHandler());

		pendingRequests.push_back(std::move(successHandler));
		pendingRequests.push_back(std::move(failureHandler));

		JSValue::global("window").call<void>("cefQuery", args);
		return promise->get_future();
	}

	void Native::query(const JsonValue& request, std::function<void(JsonValue response)> onSuccess, std::function<void(int code, std::string message)> onFailure)
	{
		auto successHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				JsonValue response = JsonValue::parse(args[0].as<std::string>());
				if (onSuccess)
					onSuccess(response);
				return JSValue::undefined();
			});

		auto failureHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				int code = args[0].as<int>();
				std::string message = args[1].as<std::string>();
				if (onFailure)
					onFailure(code, message);
				return JSValue::undefined();
			});

		JSValue args = JSValue::object();
		args.set("request", request.to_json());
		args.set("persistent", false);
		args.set("onSuccess", successHandler->getHandler());
		args.set("onFailure", failureHandler->getHandler());

		pendingRequests.push_back(std::move(successHandler));
		pendingRequests.push_back(std::move(failureHandler));

		JSValue::global("window").call<void>("cefQuery", args);
	}

	int Native::subscribe(const JsonValue& request, std::function<void(JsonValue message)> onMessage, std::function<void(int code, std::string message)> onUnsubscribe)
	{
		auto successHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				JsonValue response = JsonValue::parse(args[0].as<std::string>());
				if (onMessage)
					onMessage(response);
				return JSValue::undefined();
			});

		auto failureHandler = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
			{
				int code = args[0].as<int>();
				std::string message = args[1].as<std::string>();
				if (onUnsubscribe)
					onUnsubscribe(code, message);
				return JSValue::undefined();
			});

		JSValue args = JSValue::object();
		args.set("request", request.to_json());
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
}
