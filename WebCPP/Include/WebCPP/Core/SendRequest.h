#pragma once

#include "JSValue.h"
#include "JsonValue.h"
#include <functional>

namespace web
{
	void sendRequest(std::string url, const JsonValue& request, std::function<void(JsonValue response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
	void setDefaultRequestErrorHandler(std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler);
	void callDefaultRequestErrorHandler(int statusCode, std::string contentType, std::string body);
	void sendRequestSaveResponse(std::string url, const JsonValue& request, const std::string& filename, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
}
