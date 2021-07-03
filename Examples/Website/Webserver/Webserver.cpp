
#include "Webserver.h"
#include "ZipReader.h"
#include "UTF16.h"
#include "File.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <thread>
#include <array>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#undef min
#undef max
#include <http.h>

#pragma comment(lib, "httpapi.lib")

class WebserverImpl : public Webserver
{
public:
	WebserverImpl()
	{
		extensionMimeTypes["js"] = "text/javascript";
		extensionMimeTypes["css"] = "text/css";
		extensionMimeTypes["txt"] = "text/plain";
		extensionMimeTypes["html"] = "text/html";
		extensionMimeTypes["xml"] = "text/xml";
		extensionMimeTypes["svg"] = "image/svg+xml";
		extensionMimeTypes["png"] = "image/png";
		extensionMimeTypes["ico"] = "image/vnd.microsoft.icon";
		extensionMimeTypes["json"] = "application/json";
		extensionMimeTypes["zip"] = "application/zip";
		extensionMimeTypes["eot"] = "application/vnd.ms-fontobject";
		extensionMimeTypes["otf"] = "font/otf";
		extensionMimeTypes["ttf"] = "font/ttf";
		extensionMimeTypes["woff"] = "font/woff";
		extensionMimeTypes["wasm"] = "application/wasm";

		ULONG result = HttpInitialize(HTTPAPI_VERSION_2, HTTP_INITIALIZE_SERVER, nullptr);
		if (result != NO_ERROR)
			throw std::runtime_error("HttpInitialize failed");

		result = HttpCreateServerSession(HTTPAPI_VERSION_2, &serverSessionId, 0);
		if (result != NO_ERROR)
			throw std::runtime_error("HttpCreateServerSession failed");

		result = HttpCreateUrlGroup(serverSessionId, &urlGroupId, 0);
		if (result != NO_ERROR)
			throw std::runtime_error("HttpCreateUrlGroup failed");

		result = HttpAddUrlToUrlGroup(urlGroupId, fullyQualifiedUrl.c_str(), 0, 0);
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

		io = CreateThreadpoolIo(requestQueue, &WebserverImpl::ioCompletionCallback, this, nullptr);
		if (io == 0)
			throw std::runtime_error("CreateThreadpoolIo failed");

		unsigned int requestsCounter = std::max(std::thread::hardware_concurrency(), 1U) * 4;
		for (int i = 0; i < requestsCounter; i++)
		{
			ReceiveRequest::start(this);
		}
	}

	~WebserverImpl()
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

	static void CALLBACK ioCompletionCallback(PTP_CALLBACK_INSTANCE instance, PVOID context, PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io)
	{
		WebserverImpl* self = (WebserverImpl*)context;
		IoContext* ioContext = CONTAINING_RECORD(overlapped, IoContext, overlapped);
		ioContext->ioCompletion(self, ioResult, numberOfBytesTransferred);
	}

	class IoContext
	{
	public:
		OVERLAPPED overlapped = {};
		virtual void ioCompletion(WebserverImpl* webserver, ULONG ioResult, ULONG bytesReceived) = 0;
	};

	class ReceiveRequest : public IoContext
	{
	public:
		static void start(WebserverImpl* webserver)
		{
			ReceiveRequest* context = new ReceiveRequest();
			context->receive(webserver);
		}

	private:
		ReceiveRequest()
		{
			buffer.resize(sizeof(HTTP_REQUEST) + 2048);
		}

		void receive(WebserverImpl* webserver)
		{
			StartThreadpoolIo(webserver->io);
			ULONG bytesReceived = 0;
			ULONG result = HttpReceiveHttpRequest(webserver->requestQueue, HTTP_NULL_ID, 0, (HTTP_REQUEST*)buffer.data(), (ULONG)buffer.size(), &bytesReceived, &overlapped);
			if (result != ERROR_IO_PENDING && result != NO_ERROR)
			{
				CancelThreadpoolIo(webserver->io);
				ioCompletion(webserver, result, bytesReceived);
			}
		}

		void ioCompletion(WebserverImpl* webserver, ULONG ioResult, ULONG bytesReceived) override
		{
			auto request = (HTTP_REQUEST*)buffer.data();
			if (ioResult == NO_ERROR)
			{
				if (request->Verb == HttpVerbGET)
				{
					try
					{
						std::string urlPath = from_utf16(request->CookedUrl.pAbsPath);
						if (urlPath == "/")
							urlPath = "index.html";
						if (!urlPath.empty() && urlPath.front() == '/')
							urlPath = urlPath.substr(1);

						auto zip = ZipReader::open(FilePath::combine(Directory::exe_path(), "Website.webpkg"));
						std::vector<uint8_t> body;

						try
						{
							body = zip->read_all_bytes(urlPath);

							std::string contentType = "application/octet-stream";
							auto it = webserver->extensionMimeTypes.find(FilePath::extension(urlPath));
							if (it != webserver->extensionMimeTypes.end())
								contentType = it->second;

							SendResponse::start(webserver, request, 200, "OK", contentType, body.data(), body.size());
						}
						catch (const std::exception& e)
						{
							std::string body = e.what();
							SendResponse::start(webserver, request, 404, "Not found", "text/plain", body.data(), body.size());
						}
					}
					catch (const std::exception& e)
					{
						std::string body = e.what();
						SendResponse::start(webserver, request, 500, "Internal Server Error", "text/plain", body.data(), body.size());
					}
				}
				/*else if (request->Verb == HttpVerbPOST)
				{
					// To do: read request body
					std::string body = "Moo post";
					webserver->sendResponse(request, 200, "OK", body.data(), body.size());
				}*/
				else
				{
					std::string body = "Unimplemented request type";
					SendResponse::start(webserver, request, 503, "Not Implemented", "text/plain", body.data(), body.size());
				}
			}
			else if (ioResult == ERROR_MORE_DATA)
			{
				buffer.resize(bytesReceived);
				receive(webserver);
			}

			delete this;
		}

		std::vector<uint8_t> buffer;
	};

	class SendResponse : public IoContext
	{
	public:
		static void start(WebserverImpl* webserver, HTTP_REQUEST* request, int statusCode, std::string statusText, std::string contentType, const void* data, size_t size)
		{
			SendResponse* context = new SendResponse(statusCode, statusText, contentType, data, size);
			context->send(webserver, request);
		}

	private:
		SendResponse(int statusCode, std::string statusText, std::string contentType, const void* d, size_t size) : statusCode(statusCode), statusText(statusText), contentType(contentType)
		{
			data.resize(size);
			memcpy(data.data(), d, size);
		}

		void send(WebserverImpl* webserver, HTTP_REQUEST* request)
		{
			StartThreadpoolIo(webserver->io);

			HTTP_DATA_CHUNK dataChunk = {};
			dataChunk.DataChunkType = HttpDataChunkFromMemory;
			dataChunk.FromMemory.pBuffer = (PVOID)data.data();
			dataChunk.FromMemory.BufferLength = (ULONG)data.size();

			HTTP_RESPONSE response = {};
			response.StatusCode = statusCode;
			response.pReason = statusText.data();
			response.ReasonLength = (USHORT)statusText.size();
			response.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = contentType.data();
			response.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = (USHORT)contentType.size();
			response.EntityChunkCount = 1;
			response.pEntityChunks = &dataChunk;

			ULONG bytesSent = 0;
			ULONG result = HttpSendHttpResponse(webserver->requestQueue, request->RequestId, 0, &response, nullptr, &bytesSent, nullptr, 0, &overlapped, nullptr);
			if (result != ERROR_IO_PENDING && result != NO_ERROR)
			{
				CancelThreadpoolIo(webserver->io);
				ioCompletion(webserver, result, bytesSent);
			}
		}

		void ioCompletion(WebserverImpl* webserver, ULONG ioResult, ULONG bytesReceived) override
		{
			if (!webserver->stopFlag)
				ReceiveRequest::start(webserver);
		}

		int statusCode;
		std::string statusText;
		std::string contentType;
		std::vector<uint8_t> data;
	};

	HTTP_SERVER_SESSION_ID serverSessionId = 0;
	HTTP_URL_GROUP_ID urlGroupId = 0;
	PTP_IO io = 0;
	HANDLE requestQueue = 0;
	std::atomic<bool> stopFlag;

	std::map<std::string, std::string> extensionMimeTypes;
	std::wstring fullyQualifiedUrl = L"http://+:80/";
};

std::unique_ptr<Webserver> Webserver::create()
{
	return std::make_unique<WebserverImpl>();
}
