#pragma once

#include <vector>
#include <functional>
#include "JSValue.h"

class JSCallback;

class Native
{
public:
	static Native* get();

	void query(JSValue request, std::function<void(JSValue response)> onSuccess = {}, std::function<void(int code, JSValue message)> onFailure = {});
	int subscribe(JSValue request, std::function<void(JSValue message)> onMessage, std::function<void(int code, JSValue message)> onUnsubscribe = {});
	void unsubscribe(int id);

private:
	std::vector<std::unique_ptr<JSCallback>> pendingRequests;
};
