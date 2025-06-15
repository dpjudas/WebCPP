#pragma once

#include <emscripten/val.h>

namespace web
{
	using JSValue = emscripten::val;
	using emscripten::vecFromJSArray;

	static std::string stringify(const JSValue& value)
	{
		return JSValue::global("JSON").call<JSValue>("stringify", value).as<std::string>();
	}

	static JSValue toJSValue(const std::string& value)
	{
		return JSValue::global("JSON").call<JSValue>("parse", value);
	}
}
