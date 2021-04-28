
#include "SendRequest.h"
#include "JSCallback.h"
#include "Navigation.h"

namespace
{
	std::vector<std::unique_ptr<JSCallback>> pendingRequests;
	std::function<void(int statusCode, std::string contentType, std::string body)> defaultErrorHandler;
}

void sendRequest(std::string url, JSValue request, std::function<void(JSValue response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler)
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
					responseHandler(JSValue::global("JSON").call<JSValue>("parse", responseText));
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
	httpRequest.call<void>("send", JSValue::global("JSON").call<JSValue>("stringify", request));
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
