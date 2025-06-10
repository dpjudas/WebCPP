#pragma once

#include "HttpSysIoContext.h"
#include "Webserver/WebHeaderName.h"

class HttpSysSendResponse : public HttpSysIoContext
{
public:
	static void start(HttpSysWebserver* webserver, HTTP_REQUEST* request, int statusCode, std::string statusText, std::map<WebHeaderName, std::string> headers, const void* data, size_t size);

private:
	HttpSysSendResponse(int statusCode, std::string statusText, std::map<WebHeaderName, std::string> headersarg, const void* d, size_t size);

	void addHeaders(HTTP_RESPONSE* response);
	void send(HttpSysWebserver* webserver, HTTP_REQUEST* request);
	void ioCompletion(HttpSysWebserver* webserver, ULONG ioResult, ULONG bytesReceived) override;

	int statusCode;
	std::string statusText;
	std::map<WebHeaderName, std::string> headers;
	std::vector<uint8_t> data;
};
