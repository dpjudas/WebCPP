
#include "Precomp.h"
#include "HttpSysSendResponse.h"
#include "HttpSysReceiveRequest.h"

void HttpSysSendResponse::start(HttpSysWebserver* webserver, HTTP_REQUEST* request, int statusCode, std::string statusText, std::map<WebHeaderName, std::string> headers, const void* data, size_t size)
{
	HttpSysSendResponse* context = new HttpSysSendResponse(statusCode, statusText, std::move(headers), data, size);
	context->send(webserver, request);
}

HttpSysSendResponse::HttpSysSendResponse(int statusCode, std::string statusText, std::map<WebHeaderName, std::string> headersarg, const void* d, size_t size) : statusCode(statusCode), statusText(statusText), headers(std::move(headersarg))
{
	data.resize(size);
	memcpy(data.data(), d, size);
}

void HttpSysSendResponse::addHeaders(HTTP_RESPONSE* response)
{
	static std::map<WebHeaderName, int> knownResponseHeaders =
	{
		{ "Cache-Control", HttpHeaderCacheControl },
		{ "Connection", HttpHeaderConnection },
		{ "Date", HttpHeaderDate },
		{ "Keep-Alive", HttpHeaderKeepAlive },
		{ "Pragma", HttpHeaderPragma },
		{ "Trailer", HttpHeaderTrailer },
		{ "Transfer-Encoding", HttpHeaderTransferEncoding },
		{ "Upgrade", HttpHeaderUpgrade },
		{ "Via", HttpHeaderVia },
		{ "Warning", HttpHeaderWarning },
		{ "Allow", HttpHeaderAllow },
		{ "Content-Length", HttpHeaderContentLength },
		{ "Content-Type", HttpHeaderContentType },
		{ "Content-Encoding", HttpHeaderContentEncoding },
		{ "Content-Language", HttpHeaderContentLanguage },
		{ "Content-Location", HttpHeaderContentLocation },
		{ "Md5", HttpHeaderContentMd5 },
		{ "Range", HttpHeaderContentRange },
		{ "Expire", HttpHeaderExpires },
		{ "Last-Modified", HttpHeaderLastModified },
		{ "Accept-Ranges", HttpHeaderAcceptRanges },
		{ "Age", HttpHeaderAge },
		{ "E-Tag", HttpHeaderEtag },
		{ "Location", HttpHeaderLocation },
		{ "Proxy-Authenticate", HttpHeaderProxyAuthenticate },
		{ "Retry-After", HttpHeaderRetryAfter },
		{ "Server", HttpHeaderServer },
		{ "Set-Cookie", HttpHeaderSetCookie },
		{ "Vary", HttpHeaderVary },
		{ "Www-Authenticate", HttpHeaderWwwAuthenticate }
	};

	for (auto& header : headers)
	{
		auto it = knownResponseHeaders.find(header.first);
		if (it != knownResponseHeaders.end())
		{
			response->Headers.KnownHeaders[it->second].pRawValue = header.second.data();
			response->Headers.KnownHeaders[it->second].RawValueLength = (USHORT)header.second.size();
		}
		else
		{
			// To do: add to unknown headers list
		}
	}
}

void HttpSysSendResponse::send(HttpSysWebserver* webserver, HTTP_REQUEST* request)
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
	addHeaders(&response);
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

void HttpSysSendResponse::ioCompletion(HttpSysWebserver* webserver, ULONG ioResult, ULONG bytesReceived)
{
	if (!webserver->stopFlag)
		HttpSysReceiveRequest::start(webserver);
}
