
#include "Precomp.h"
#include "WebContext.h"
#include "Json/JsonValue.h"

JsonValue WebContext::getJsonRequest() const
{
	//if (Request.GetHeader("Content-Type") != "application/json")
	//	throw std::runtime_error("Unexpected content-type header for json: '" + Request.GetHeader("Content-Type") + "'");

	return JsonValue::parse(std::string((const char*)request.content->data(), request.content->size()));
}

void WebContext::setJsonResponse(const JsonValue& value, int statusCode, const std::string& statusText)
{
	setTextResponse(value.to_json(), "application/json", statusCode, statusText);
}

void WebContext::setHtmlResponse(const std::string& html, int statusCode, const std::string& statusText)
{
	setTextResponse(html, "text/html", statusCode, statusText);
}

void WebContext::setTextResponse(const std::string& text, const std::string& contentType, int statusCode, const std::string& statusText)
{
	setBinaryResponse(text.data(), text.size(), contentType, statusCode, statusText);
}

void WebContext::setBinaryResponse(const void* data, size_t size, const std::string& contentType, int statusCode, const std::string& statusText)
{
	setBinaryResponse(DataBuffer::create(data, size), contentType, statusCode, statusText);
}

void WebContext::setBinaryResponse(std::shared_ptr<DataBuffer> data, const std::string& contentType, int statusCode, const std::string& statusText)
{
	response.status.code = statusCode;
	response.status.text = statusText;
	response.headers["Content-Type"] = contentType;
	response.headers["Content-Length"] = std::to_string(data->size());
	response.content = std::move(data);
	handled = true;
}

void WebContext::setNoContentResponse(int statusCode, const std::string& statusText)
{
	response.status.code = statusCode;
	response.status.text = statusText;
	handled = true;
}

void WebContext::redirect(const std::string& url)
{
	setTextResponse("Redirected", "text/plain", 302, "Redirected");
	response.headers["Location"] = url;
}
