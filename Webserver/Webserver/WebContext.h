#pragma once

#include "WebRequest.h"
#include "WebResponse.h"

class JsonValue;
class DataBuffer;

class WebContext
{
public:
	// Path relative to the module handling the request
	std::string requestedPath;

	// The received web request
	WebRequest request;

	// The response to the request
	WebResponse response;

	// Set to true if the request was handled by the module
	bool handled = false;

	JsonValue getJsonRequest() const;

	void setJsonResponse(const JsonValue& value, int statusCode = 200, const std::string& statusText = "OK");
	void setHtmlResponse(const std::string& html, int statusCode = 200, const std::string& statusText = "OK");
	void setTextResponse(const std::string& text, const std::string& contentType = "text/plain", int statusCode = 200, const std::string& statusText = "OK");
	void setBinaryResponse(const void* data, size_t size, const std::string& contentType = "application/octet-stream", int statusCode = 200, const std::string& statusText = "OK");
	void setBinaryResponse(std::shared_ptr<DataBuffer> data, const std::string& contentType = "application/octet-stream", int statusCode = 200, const std::string& statusText = "OK");
	void setNoContentResponse(int statusCode = 204, const std::string& statusText = "No Content");

	void redirect(const std::string& url);
};
