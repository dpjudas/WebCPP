#pragma once

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include "JSValue.h"
#include "Task.h"

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
		static void query(const std::string& request, std::function<void(std::string response)> onSuccess, std::function<void(int code, std::string message)> onFailure = {});
		static task<std::string> query(const std::string& request);
		static int subscribe(const std::string& request, std::function<void(std::string message)> onMessage, std::function<void(int code, std::string message)> onUnsubscribe = {});
		static void unsubscribe(int id);
	};
}
