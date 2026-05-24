#pragma once

#include "JSValue.h"
#include "JsonValue.h"
#include "Task.h"
#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace web
{
	class WebSendRequestException : public std::exception
	{
	public:
		WebSendRequestException(int statusCode, std::string statusText, std::string message, JsonValue body) : statusCode(statusCode), statusText(std::move(statusText)), message(std::move(message)), body(std::move(body)) {}

		char const* what() const noexcept override { return message.c_str(); }

		int getStatusCode() const { return statusCode; }
		const std::string& getStatusText() const { return statusText; }
		const std::string& getMessage() const { return message; }
		const JsonValue& getBody() const { return body; }

	private:
		int statusCode = 0;
		std::string statusText;
		std::string message;
		JsonValue body;
	};


	task<JsonValue> sendRequest(std::string url, const JsonValue& request);
	task<std::vector<uint8_t>> sendRequestBinary(std::string url, const JsonValue& request);

	void sendRequest(std::string url, const JsonValue& request, std::function<void(JsonValue response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
	void sendRequestBinary(std::string url, const JsonValue& request, std::function<void(std::vector<uint8_t> response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
	void setDefaultRequestErrorHandler(std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler);
	void callDefaultRequestErrorHandler(int statusCode, std::string contentType, std::string body);
	void sendRequestSaveResponse(std::string url, const JsonValue& request, const std::string& filename, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
}
