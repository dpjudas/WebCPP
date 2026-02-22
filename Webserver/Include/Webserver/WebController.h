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

	class WebControllerException : public std::exception
	{
	public:
		WebControllerException(int statusCode, std::string statusText, std::string message) : statusCode(statusCode), statusText(std::move(statusText)), message(std::move(message)) {}

		char const* what() const override { return message.c_str(); }

		int getStatusCode() const { return statusCode; }
		const std::string& getStatusText() const { return statusText; }
		const std::string& getMessage() const { return message; }

	private:
		int statusCode = 0;
		std::string statusText;
		std::string message;
	};
}
