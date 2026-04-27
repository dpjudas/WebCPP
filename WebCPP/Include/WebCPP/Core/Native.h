#pragma once

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include "JSValue.h"
#include "Task.h"
#include "JsonValue.h"

namespace web
{
	class NativeException : public std::runtime_error
	{
	public:
		NativeException(int code, const std::string& message) : std::runtime_error(message), code(code) {}
		int code = 0;
	};

	class Native
	{
	public:
		static bool isApp();
		static void query(const JsonValue& request, std::function<void(JsonValue response)> onSuccess, std::function<void(int code, std::string message)> onFailure = {});
		static task<JsonValue> query(const JsonValue& request);
		static int subscribe(const JsonValue& request, std::function<void(JsonValue message)> onMessage, std::function<void(int code, std::string message)> onUnsubscribe = {});
		static void unsubscribe(int id);
	};
}
