#pragma once

#include "WebModule.h"
#include "Json/JsonValue.h"

class WebController : public WebModule
{
public:
	WebController(std::string baseRoute);

	void process(WebContext* context) override;

protected:
	template<typename T>
	void bind(const std::string& path, JsonValue(T::* func)(JsonValue))
	{
		bindings[path] = [=](JsonValue request) -> JsonValue { return (*static_cast<T*>(this).*func)(std::move(request)); };
	}

	std::map<std::string, std::function<JsonValue(JsonValue)>> bindings;
};
