
#include "WebCPP/Core/Clipboard.h"
#include "WebCPP/Core/JSCallback.h"
#include "WebCPP/Core/JSValue.h"
#include "WebCPP/Core/Permissions.h"

void Clipboard::setText(const std::string& text)
{
	JSValue promise = JSValue::global("navigator")["clipboard"].call<JSValue>("writeText", text);
	auto callback = new JSCallback([](JSValue) { return JSValue::undefined(); }, true);
	promise.call<void>("then", callback->getHandler());
}

void Clipboard::getText(std::function<void(std::string text)> onResult)
{
	JSValue promise = JSValue::global("navigator")["clipboard"].call<JSValue>("readText");
	auto callback = new JSCallback([=](JSValue args) { onResult(args[0].as<std::string>()); return JSValue::undefined(); }, true);
	promise.call<void>("then", callback->getHandler());
}
