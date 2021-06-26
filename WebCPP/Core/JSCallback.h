#pragma once

#include <functional>
#include "JSValue.h"

class JSCallback
{
public:
	JSCallback(std::function<JSValue(JSValue args)> callback, bool deleteOnInvoke = false);
	~JSCallback();

	JSValue getHandler();
	void invoke();

private:
	std::function<JSValue(JSValue args)> callback;
	JSValue handle;
	bool deleteOnInvoke = false;

	JSCallback(const JSCallback&) = delete;
	JSCallback& operator=(const JSCallback&) = delete;
};
