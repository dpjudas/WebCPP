#pragma once

#include <string>

#ifndef INTELLISENSE
#include <emscripten/val.h>
#endif

namespace web
{
#ifndef INTELLISENSE
	using JSValue = emscripten::val;
	using emscripten::vecFromJSArray;
#else
	class JSValue
	{
	public:
		static JSValue global(const std::string& name);
		static JSValue object();
		static JSValue undefined();
		template<typename T> static JSValue array(const std::vector<T>& items);
		template<typename... Args> JSValue new_(Args... args) const;
		JSValue operator[](const std::string& name) const;
		template<typename... Args>JSValue operator()(Args... args) const;
		template<typename T> void set(const std::string& name, T value);
		template<typename RetVal, typename... Args> RetVal call(const std::string& name, Args... args) const;
		template<typename RetVal> RetVal as() const;
		bool isNull() const;
		bool isString() const;
		bool isNumber() const;
		bool isUndefined() const;
	};
	template<typename T> std::vector<T> vecFromJSArray(JSValue);
#endif

	static std::string stringify(const JSValue& value)
	{
		return JSValue::global("JSON").call<JSValue>("stringify", value).as<std::string>();
	}

	static JSValue toJSValue(const std::string& value)
	{
		return JSValue::global("JSON").call<JSValue>("parse", value);
	}
}
