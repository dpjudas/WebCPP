
#include "SendRequest.h"
#include "JSCallback.h"
#include "Navigation.h"
#include "Timer.h"

namespace
{
	std::vector<std::unique_ptr<JSCallback>> pendingRequests;
	std::function<void(int statusCode, std::string contentType, std::string body)> defaultErrorHandler;
}

void sendRequest(std::string url, const JsonValue& request, std::function<void(JsonValue response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler)
{
	JSValue httpRequest = JSValue::global("XMLHttpRequest").new_();

	auto onreadystatechange = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		if (httpRequest["readyState"].as<int>() == 4 /*XMLHttpRequest.DONE*/)
		{
			int statusCode = httpRequest["status"].as<int>();
			std::string contentType = statusCode != 0 ? httpRequest.call<std::string>("getResponseHeader", std::string("Content-Type")) : "";
			JSValue responseText = httpRequest["responseText"];
			if (statusCode == 200 && contentType == "application/json")
			{
				if (responseHandler)
				{
					responseHandler(JsonValue::parse(responseText.as<std::string>()));
				}
			}
			else
			{
				if (errorHandler)
				{
					errorHandler(statusCode, contentType, responseText.isString() ? responseText.as<std::string>() : "");
				}
				else
				{
					callDefaultRequestErrorHandler(statusCode, contentType, responseText.isString() ? responseText.as<std::string>() : "");
				}
			}
		}

		return JSValue::undefined();
	});

	httpRequest.set("onreadystatechange", onreadystatechange->getHandler());
	pendingRequests.push_back(std::move(onreadystatechange));

	httpRequest.call<void>("open", std::string("POST"), url, true);
	std::string accessToken = Navigation::getAccessToken();
	if (!accessToken.empty())
		httpRequest.call<void>("setRequestHeader", std::string("Authorization"), "Bearer " + accessToken);
	httpRequest.call<void>("setRequestHeader", std::string("Content-Type"), std::string("application/json"));
	httpRequest.call<void>("send", request.to_json());
}

void setDefaultRequestErrorHandler(std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler)
{
	defaultErrorHandler = std::move(errorHandler);
}

void callDefaultRequestErrorHandler(int statusCode, std::string contentType, std::string body)
{
	if (defaultErrorHandler)
	{
		defaultErrorHandler(statusCode, std::move(contentType), std::move(body));
	}
}

void sendRequestSaveResponse(std::string url, const JsonValue& request, const std::string& filename, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler)
{
	JSValue httpRequest = JSValue::global("XMLHttpRequest").new_();

	auto onreadystatechange = std::make_unique<JSCallback>([=](JSValue args) -> JSValue
	{
		if (httpRequest["readyState"].as<int>() == 4 /*XMLHttpRequest.DONE*/)
		{
			int statusCode = httpRequest["status"].as<int>();
			std::string contentType = statusCode != 0 ? httpRequest.call<std::string>("getResponseHeader", std::string("Content-Type")) : "";
			if (statusCode == 200)
			{
				JSValue blob = httpRequest["response"];
				JSValue fileURL = JSValue::global("URL").call<JSValue>("createObjectURL", blob);

				JSValue element = JSValue::global("document").call<JSValue>("createElement", std::string("a"));
				element.set("download", filename);
				element.set("rel", std::string("noopener")); // prevents tabnabbing, supposedly
				element.set("href", fileURL);

				setTimeout([=]() { JSValue::global("URL").call<void>("revokeObjectURL", fileURL); }, 60000);
				element.call<void>("click");
			}
			else
			{
				JSValue responseText = httpRequest["responseText"];
				if (errorHandler)
				{
					errorHandler(statusCode, contentType, responseText.isString() ? responseText.as<std::string>() : "");
				}
				else
				{
					callDefaultRequestErrorHandler(statusCode, contentType, responseText.isString() ? responseText.as<std::string>() : "");
				}
			}
		}

		return JSValue::undefined();
	});

	httpRequest.set("onreadystatechange", onreadystatechange->getHandler());
	pendingRequests.push_back(std::move(onreadystatechange));

	httpRequest.call<void>("open", std::string("POST"), url, true);
	httpRequest.set("responseType", "blob");
	std::string accessToken = Navigation::getAccessToken();
	if (!accessToken.empty())
		httpRequest.call<void>("setRequestHeader", std::string("Authorization"), "Bearer " + accessToken);
	httpRequest.call<void>("setRequestHeader", std::string("Content-Type"), std::string("application/json"));
	httpRequest.call<void>("send", request.to_json());
}
