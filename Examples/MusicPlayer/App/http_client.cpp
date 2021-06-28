
#include "http_client.h"
#include "include/cef_urlrequest.h"
#include <mutex>
#include <condition_variable>
#include <chrono>

class HttpURLRequestClient : public CefURLRequestClient
{
public:
	HttpURLRequestClient(int timeout, uint64_t maxbuffersize, bool isbytes) : timeout(timeout), maxbuffersize(maxbuffersize), isbytes(isbytes)
	{
	}

	void OnRequestComplete(CefRefPtr<CefURLRequest> request) override
	{
		std::unique_lock<std::mutex> lock(mutex);
		done = true;
		lock.unlock();
		condvar.notify_all();
	}

	void OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override
	{
		condvar.notify_all();
	}

	void OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override
	{
		condvar.notify_all();
	}

	void OnDownloadData(CefRefPtr<CefURLRequest> request, const void* data, size_t data_length) override
	{
		if (isbytes)
			bufferbytes.insert(bufferbytes.end(), static_cast<const char*>(data), static_cast<const char*>(data) + data_length);
		else
			bufferstring.append(static_cast<const char*>(data), data_length);
		condvar.notify_all();
	}

	bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) override
	{
		return false;
	}

	std::vector<uint8_t> GetResponseBytes(CefRefPtr<CefURLRequest> request)
	{
		WaitForResult(std::move(request));
		return std::move(bufferbytes);
	}

	std::string GetResponseString(CefRefPtr<CefURLRequest> request)
	{
		WaitForResult(std::move(request));
		return std::move(bufferstring);
	}

private:
	void WaitForResult(CefRefPtr<CefURLRequest> request)
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(mutex);
			auto predicate = [this]() -> bool { return done; };
			auto timepoint = std::chrono::milliseconds(timeout);
			bool result = condvar.wait_for(lock, timepoint, predicate);
			if (!result)
			{
				request->Cancel();
				throw HttpClientTimeoutError();
			}

			if (request->GetRequestStatus() != UR_SUCCESS)
				throw HttpClientRequestError();
		}
	}

	std::mutex mutex;
	std::condition_variable condvar;
	bool done = false;

	int timeout;
	uint64_t maxbuffersize;
	bool isbytes;

	std::vector<uint8_t> bufferbytes;
	std::string bufferstring;

	IMPLEMENT_REFCOUNTING(HttpURLRequestClient);
};

class HttpClientImpl : public HttpClient
{
public:
	int GetTimeout() override;
	void SetTimeout(int milliseconds) override;

	uint64_t GetMaxResponseContentBufferSize() override;
	void SetMaxResponseContentBufferSize(uint64_t size) override;

	std::string GetString(const std::string& requestUri) override;
	std::vector<uint8_t> GetBytes(const std::string& requestUri) override;
	std::string PostString(const std::string& requestUri, const void* postData, size_t postSize, const std::string& contentType) override;
	std::vector<uint8_t> PostBytes(const std::string& requestUri, const void* postData, size_t postSize, const std::string& contentType) override;

	int timeout = 5000;
	uint64_t maxbuffersize = 2048LL * 1024 * 1024;
};

std::unique_ptr<HttpClient> HttpClient::Create()
{
	return std::make_unique<HttpClientImpl>();
}

int HttpClientImpl::GetTimeout()
{
	return timeout;
}

void HttpClientImpl::SetTimeout(int milliseconds)
{
	timeout = milliseconds;
}

uint64_t HttpClientImpl::GetMaxResponseContentBufferSize()
{
	return maxbuffersize;
}

void HttpClientImpl::SetMaxResponseContentBufferSize(uint64_t size)
{
	maxbuffersize = size;
}

std::string HttpClientImpl::GetString(const std::string& requestUri)
{
	CefRefPtr<CefRequest> request = CefRequest::Create();
	request->SetFlags(UR_FLAG_SKIP_CACHE);
	request->SetURL(requestUri);
	request->SetMethod("GET");

	CefRefPtr<HttpURLRequestClient> client = new HttpURLRequestClient(timeout, maxbuffersize, false);
	return client->GetResponseString(CefURLRequest::Create(request, client.get(), nullptr));
}

std::vector<uint8_t> HttpClientImpl::GetBytes(const std::string& requestUri)
{
	CefRefPtr<CefRequest> request = CefRequest::Create();
	request->SetFlags(UR_FLAG_SKIP_CACHE);
	request->SetURL(requestUri);
	request->SetMethod("GET");

	CefRefPtr<HttpURLRequestClient> client = new HttpURLRequestClient(timeout, maxbuffersize, true);
	return client->GetResponseBytes(CefURLRequest::Create(request, client.get(), nullptr));
}

std::string HttpClientImpl::PostString(const std::string& requestUri, const void* postData, size_t postSize, const std::string& contentType)
{
	CefRefPtr<CefRequest> request = CefRequest::Create();
	request->SetFlags(UR_FLAG_SKIP_CACHE);
	request->SetURL(requestUri);
	request->SetMethod("POST");

	CefRequest::HeaderMap headerMap;
	headerMap.insert(std::make_pair("Content-Type", contentType.c_str()));
	request->SetHeaderMap(headerMap);

	CefRefPtr<HttpURLRequestClient> client = new HttpURLRequestClient(timeout, maxbuffersize, false);
	return client->GetResponseString(CefURLRequest::Create(request, client.get(), nullptr));
}

std::vector<uint8_t> HttpClientImpl::PostBytes(const std::string& requestUri, const void* postData, size_t postSize, const std::string& contentType)
{
	CefRefPtr<CefRequest> request = CefRequest::Create();
	request->SetFlags(UR_FLAG_SKIP_CACHE);
	request->SetURL(requestUri);
	request->SetMethod("POST");

	CefRequest::HeaderMap headerMap;
	headerMap.insert(std::make_pair("Content-Type", contentType.c_str()));
	request->SetHeaderMap(headerMap);

	CefRefPtr<HttpURLRequestClient> client = new HttpURLRequestClient(timeout, maxbuffersize, true);
	return client->GetResponseBytes(CefURLRequest::Create(request, client.get(), nullptr));
}
