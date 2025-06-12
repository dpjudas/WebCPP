#include "WebCPP/Core/Permissions.h"
#include "WebCPP/Core/JSCallback.h"
#include "WebCPP/Core/JSValue.h"

void Permissions::query(const std::string& permission, std::function<void(const std::string& state)> onThen)
{
	JSValue request = JSValue::object();
	request.set("name", permission);

	JSValue promise = JSValue::global("navigator")["permissions"].call<JSValue>("query", request);

	auto callback = new JSCallback([=](JSValue args) { onThen(args[0]["state"].as<std::string>()); return JSValue::undefined(); }, true);

	const JSValue& result = promise.call<JSValue>("then", callback->getHandler());
}
