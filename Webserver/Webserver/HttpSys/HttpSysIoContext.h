#pragma once

#include "HttpSysWebserver.h"

class HttpSysIoContext
{
public:
	OVERLAPPED overlapped = {};
	virtual void ioCompletion(HttpSysWebserver* webserver, ULONG ioResult, ULONG bytesReceived) = 0;
};
