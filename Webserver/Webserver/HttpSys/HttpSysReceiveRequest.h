#pragma once

#include "HttpSysIoContext.h"

class WebContext;

class HttpSysReceiveRequest : public HttpSysIoContext
{
public:
	static void start(HttpSysWebserver* webserver);

private:
	HttpSysReceiveRequest();
	void receive(HttpSysWebserver* webserver);
	void addHeaders(HTTP_REQUEST* request, WebContext& webctx);
	void ioCompletion(HttpSysWebserver* webserver, ULONG ioResult, ULONG bytesReceived) override;

	std::vector<uint8_t> buffer;
};
