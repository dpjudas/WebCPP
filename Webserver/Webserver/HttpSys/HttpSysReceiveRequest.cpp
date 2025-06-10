
#include "Precomp.h"
#include "HttpSysReceiveRequest.h"
#include "HttpSysSendResponse.h"
#include "Webserver/WebContext.h"
#include "Text/UTF16.h"

void HttpSysReceiveRequest::start(HttpSysWebserver* webserver)
{
	HttpSysReceiveRequest* context = new HttpSysReceiveRequest();
	context->receive(webserver);
}

HttpSysReceiveRequest::HttpSysReceiveRequest()
{
	buffer.resize(sizeof(HTTP_REQUEST) + 2048);
}

void HttpSysReceiveRequest::receive(HttpSysWebserver* webserver)
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

void HttpSysReceiveRequest::addHeaders(HTTP_REQUEST* request, WebContext& webctx)
{
	struct KnownHeader { int index; WebHeaderName name; };
	static std::vector<KnownHeader> knownRequestHeaders =
	{
		{ HttpHeaderCacheControl, "Cache-Control" },
		{ HttpHeaderConnection, "Connection" },
		{ HttpHeaderDate, "Date" },
		{ HttpHeaderKeepAlive, "Keep-Alive" },
		{ HttpHeaderPragma, "Pragma" },
		{ HttpHeaderTrailer, "Trailer" },
		{ HttpHeaderTransferEncoding, "Transfer-Encoding" },
		{ HttpHeaderUpgrade, "Upgrade" },
		{ HttpHeaderVia, "Via" },
		{ HttpHeaderWarning, "Warning" },
		{ HttpHeaderAllow, "Allow" },
		{ HttpHeaderContentLength, "Content-Length" },
		{ HttpHeaderContentType, "Content-Type" },
		{ HttpHeaderContentEncoding, "Content-Encoding" },
		{ HttpHeaderContentLanguage, "Content-Language" },
		{ HttpHeaderContentLocation, "Content-Location" },
		{ HttpHeaderContentMd5, "Md5" },
		{ HttpHeaderContentRange, "Range" },
		{ HttpHeaderExpires, "Expire" },
		{ HttpHeaderLastModified, "Last-Modified" },
		{ HttpHeaderAccept, "Accept" },
		{ HttpHeaderAcceptCharset, "Accept-Charset" },
		{ HttpHeaderAcceptEncoding, "Accept-Encoding" },
		{ HttpHeaderAcceptLanguage, "Accept-Language" },
		{ HttpHeaderAuthorization, "Authorization" },
		{ HttpHeaderCookie, "Cookie" },
		{ HttpHeaderExpect, "Expect" },
		{ HttpHeaderFrom, "From" },
		{ HttpHeaderHost, "Host" },
		{ HttpHeaderIfMatch, "If-Match" },
		{ HttpHeaderIfModifiedSince, "If-Modified-Since" },
		{ HttpHeaderIfNoneMatch, "If-None-Match" },
		{ HttpHeaderIfRange, "If-Range" },
		{ HttpHeaderIfUnmodifiedSince, "If-Unmodified-Since" },
		{ HttpHeaderMaxForwards, "Max-Forwards" },
		{ HttpHeaderProxyAuthorization, "Proxy-Authorization" },
		{ HttpHeaderReferer, "Referer" },
		{ HttpHeaderRange, "Range" },
		{ HttpHeaderTe, "Te" },
		{ HttpHeaderTranslate, "Translate" },
		{ HttpHeaderUserAgent, "User-Agent" }
	};

	for (const KnownHeader& info : knownRequestHeaders)
	{
		if (request->Headers.KnownHeaders[info.index].pRawValue && request->Headers.KnownHeaders[info.index].RawValueLength > 0)
		{
			webctx.request.headers[info.name] = std::string(request->Headers.KnownHeaders[info.index].pRawValue, request->Headers.KnownHeaders[info.index].RawValueLength);
		}
	}

	// To do: add unknown headers
}

void HttpSysReceiveRequest::ioCompletion(HttpSysWebserver* webserver, ULONG ioResult, ULONG bytesReceived)
{
	auto request = (HTTP_REQUEST*)buffer.data();
	if (ioResult == NO_ERROR)
	{
		if (request->Verb == HttpVerbGET)
		{
			try
			{
				WebContext webctx;
				webctx.request.verb = WebRequestVerb::get;
				webctx.request.url = WebRequestUrl(from_utf16(request->CookedUrl.pFullUrl));
				addHeaders(request, webctx);

				webserver->processRequest(&webctx);

				if (webctx.response.status.code == 0)
				{
					std::string body = "Not found";
					HttpSysSendResponse::start(webserver, request, 404, "Not found", { { "Content-Type", "text/plain" } }, body.data(), body.size());
				}

				if (webctx.response.content)
					HttpSysSendResponse::start(webserver, request, webctx.response.status.code, webctx.response.status.text, std::move(webctx.response.headers), webctx.response.content->data(), webctx.response.content->size());
				else
					HttpSysSendResponse::start(webserver, request, webctx.response.status.code, webctx.response.status.text, std::move(webctx.response.headers), nullptr, 0);
			}
			catch (const std::exception& e)
			{
				std::string body = e.what();
				HttpSysSendResponse::start(webserver, request, 500, "Internal Server Error", { { "Content-Type", "text/plain" } }, body.data(), body.size());
			}
		}
		else if (request->Verb == HttpVerbPOST)
		{
			// To do: read request body

			try
			{
				WebContext webctx;
				webctx.request.verb = WebRequestVerb::post;
				webctx.request.url = WebRequestUrl(from_utf16(request->CookedUrl.pFullUrl));
				addHeaders(request, webctx);

				webserver->processRequest(&webctx);

				if (webctx.response.status.code == 0)
				{
					std::string body = "Not found";
					HttpSysSendResponse::start(webserver, request, 404, "Not found", { { "Content-Type", "text/plain" } }, body.data(), body.size());
				}

				if (webctx.response.content)
					HttpSysSendResponse::start(webserver, request, webctx.response.status.code, webctx.response.status.text, std::move(webctx.response.headers), webctx.response.content->data(), webctx.response.content->size());
				else
					HttpSysSendResponse::start(webserver, request, webctx.response.status.code, webctx.response.status.text, std::move(webctx.response.headers), nullptr, 0);
			}
			catch (const std::exception& e)
			{
				std::string body = e.what();
				HttpSysSendResponse::start(webserver, request, 500, "Internal Server Error", { { "Content-Type", "text/plain" } }, body.data(), body.size());
			}
		}
		else
		{
			std::string body = "Unimplemented request type";
			HttpSysSendResponse::start(webserver, request, 503, "Not Implemented", { { "Content-Type", "text/plain" } }, body.data(), body.size());
		}
	}
	else if (ioResult == ERROR_MORE_DATA)
	{
		buffer.resize(bytesReceived);
		receive(webserver);
	}

	delete this;
}
