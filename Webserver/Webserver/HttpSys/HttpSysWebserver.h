#pragma once

#include "Webserver/Webserver.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <http.h>

class HttpSysWebserver : public Webserver
{
public:
	HttpSysWebserver();
	~HttpSysWebserver();

	void start(const std::string& listenUrl) override;

	static void CALLBACK ioCompletionCallback(PTP_CALLBACK_INSTANCE instance, PVOID context, PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io);

	HTTP_SERVER_SESSION_ID serverSessionId = 0;
	HTTP_URL_GROUP_ID urlGroupId = 0;
	PTP_IO io = 0;
	HANDLE requestQueue = 0;
	std::atomic<bool> stopFlag;

	std::wstring fullyQualifiedUrl = L"http://+:8080/";
};
