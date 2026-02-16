#pragma once

#include "WebModule.h"
#include "WebContext.h"
#include "JsonValue.h"
#include <map>
#include <string>
#include <utility>
#include <functional>

namespace web
{
	template<typename T>
	struct WebControllerArgTrait
	{
		static T fromContext(WebContext* ctx) { return T(ctx); }
	};

	class WebController : public WebModule
	{
	public:
		WebController(std::string baseRoute);

		void process(WebContext* context) override;

	protected:
		template<typename T>
		void bind(const std::string& path, JsonValue(T::* func)(JsonValue))
		{
			bindings[path] = [=,this](WebContext* context) -> JsonValue { return (*static_cast<T*>(this).*func)(context->getJsonRequest()); };
		}

		template<typename T, typename P>
		void bind(const std::string& path, JsonValue(T::* func)(JsonValue, P))
		{
			bindings[path] = [=, this](WebContext* context) -> JsonValue { return (*static_cast<T*>(this).*func)(context->getJsonRequest(), web::WebControllerArgTrait<P>::fromContext(context)); };
		}

		std::map<std::string, std::function<JsonValue(WebContext*)>> bindings;
	};
}
