#pragma once

#include <vector>
#include <functional>
#include "JSValue.h"

namespace web
{
	class Native
	{
	public:
		static bool isApp();
		static void query(JSValue request, std::function<void(JSValue response)> onSuccess = {}, std::function<void(int code, JSValue message)> onFailure = {});
		static int subscribe(JSValue request, std::function<void(JSValue message)> onMessage, std::function<void(int code, JSValue message)> onUnsubscribe = {});
		static void unsubscribe(int id);
	};
}
