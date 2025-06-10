
#include "Precomp.h"
#include "HttpSysWebserver.h"
#include "HttpSysReceiveRequest.h"
#include "Text/UTF16.h"
#include <algorithm>
#include <thread>

#pragma comment(lib, "httpapi.lib")

HttpSysWebserver::HttpSysWebserver()
{
	ULONG result = HttpInitialize(HTTPAPI_VERSION_2, HTTP_INITIALIZE_SERVER, nullptr);
	if (result != NO_ERROR)
		throw std::runtime_error("HttpInitialize failed");

	result = HttpCreateServerSession(HTTPAPI_VERSION_2, &serverSessionId, 0);
	if (result != NO_ERROR)
		throw std::runtime_error("HttpCreateServerSession failed");

	result = HttpCreateUrlGroup(serverSessionId, &urlGroupId, 0);
	if (result != NO_ERROR)
		throw std::runtime_error("HttpCreateUrlGroup failed");
}

HttpSysWebserver::~HttpSysWebserver()
{
	stopFlag = true;
	HttpShutdownRequestQueue(requestQueue);
	WaitForThreadpoolIoCallbacks(io, FALSE);
	HttpCloseRequestQueue(requestQueue);
	CloseThreadpoolIo(io);
	HttpCloseUrlGroup(urlGroupId);
	HttpCloseServerSession(serverSessionId);
	HttpTerminate(HTTP_INITIALIZE_SERVER, nullptr);
}

void HttpSysWebserver::start(const std::string& listenUrl)
{
	fullyQualifiedUrl = to_utf16(listenUrl);

	ULONG result = HttpAddUrlToUrlGroup(urlGroupId, fullyQualifiedUrl.c_str(), 0, 0);
	if (result != NO_ERROR)
		throw std::runtime_error("HttpAddUrlToUrlGroup failed");

	result = HttpCreateRequestQueue(HTTPAPI_VERSION_2, nullptr, nullptr, 0, &requestQueue);
	if (result != NO_ERROR)
		throw std::runtime_error("HttpCreateRequestQueue failed");

	HTTP_BINDING_INFO bindInfo = {};
	bindInfo.Flags.Present = 1;
	bindInfo.RequestQueueHandle = requestQueue;
	result = HttpSetUrlGroupProperty(urlGroupId, HttpServerBindingProperty, &bindInfo, sizeof(HTTP_BINDING_INFO));
	if (result != NO_ERROR)
		throw std::runtime_error("HttpSetUrlGroupProperty failed");

	io = CreateThreadpoolIo(requestQueue, &HttpSysWebserver::ioCompletionCallback, this, nullptr);
	if (io == 0)
		throw std::runtime_error("CreateThreadpoolIo failed");

	unsigned int requestsCounter = std::max(std::thread::hardware_concurrency(), 1U) * 4;
	for (unsigned int i = 0; i < requestsCounter; i++)
	{
		HttpSysReceiveRequest::start(this);
	}
}

void HttpSysWebserver::ioCompletionCallback(PTP_CALLBACK_INSTANCE instance, PVOID context, PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io)
{
	HttpSysWebserver* self = (HttpSysWebserver*)context;
	HttpSysIoContext* ioContext = CONTAINING_RECORD(overlapped, HttpSysIoContext, overlapped);
	ioContext->ioCompletion(self, ioResult, (ULONG)numberOfBytesTransferred);
}
