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
			bindings[path] = [=,this](WebContext* context) { context->setJsonResponse((*static_cast<T*>(this).*func)(context->getJsonRequest())); };
		}

		template<typename T, typename P>
		void bind(const std::string& path, JsonValue(T::* func)(JsonValue, P))
		{
			bindings[path] = [=, this](WebContext* context) { context->setJsonResponse((*static_cast<T*>(this).*func)(context->getJsonRequest(), web::WebControllerArgTrait<P>::fromContext(context))); };
		}

		template<typename T>
		void bind(const std::string& path, void(T::* func)(WebContext* context))
		{
			bindings[path] = [=, this](WebContext* context) { (*static_cast<T*>(this).*func)(context); };
		}

		template<typename T, typename P>
		void bind(const std::string& path, void(T::* func)(WebContext*, P))
		{
			bindings[path] = [=, this](WebContext* context) { (*static_cast<T*>(this).*func)(context, web::WebControllerArgTrait<P>::fromContext(context)); };
		}

		std::map<std::string, std::function<void(WebContext*)>> bindings;
	};

	class WebControllerException : public std::exception
	{
	public:
		// retryAfterSeconds is optional (0 = not set) - when positive, WebController::process() sets a Retry-After response header with this value
		WebControllerException(int statusCode, std::string statusText, std::string message, int retryAfterSeconds = 0) : statusCode(statusCode), statusText(std::move(statusText)), message(std::move(message)), retryAfterSeconds(retryAfterSeconds) {}

		char const* what() const noexcept override { return message.c_str(); }

		int getStatusCode() const { return statusCode; }
		const std::string& getStatusText() const { return statusText; }
		const std::string& getMessage() const { return message; }
		int getRetryAfterSeconds() const { return retryAfterSeconds; }

	private:
		int statusCode = 0;
		std::string statusText;
		std::string message;
		int retryAfterSeconds = 0;
	};
}
