
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

class HttpClient
{
public:
	static std::unique_ptr<HttpClient> Create();

	virtual ~HttpClient() = default;

	virtual int GetTimeout() = 0;
	virtual void SetTimeout(int milliseconds) = 0;

	virtual uint64_t GetMaxResponseContentBufferSize() = 0;
	virtual void SetMaxResponseContentBufferSize(uint64_t size) = 0;

	virtual std::string GetString(const std::string& requestUri) = 0;
	virtual std::vector<uint8_t> GetBytes(const std::string& requestUri) = 0;

	virtual std::string PostString(const std::string& requestUri, const void *postData, size_t postSize, const std::string& contentType = "application/json") = 0;
	virtual std::vector<uint8_t> PostBytes(const std::string& requestUri, const void* postData, size_t postSize, const std::string& contentType = "application/json") = 0;

	std::string PostString(const std::string& requestUri, const std::string& postData, const std::string& contentType = "application/json")
	{
		return PostString(requestUri, postData.data(), postData.size(), contentType);
	}

	std::vector<uint8_t> PostBytes(const std::string& requestUri, const std::string& postData, const std::string& contentType = "application/json")
	{
		return PostBytes(requestUri, postData.data(), postData.size(), contentType);
	}
};

class HttpClientError : public std::runtime_error
{
public:
	HttpClientError(const char* text) : std::runtime_error(text) { }
	HttpClientError(std::string text) : std::runtime_error(std::move(text)) { }
};

class HttpClientTimeoutError : public HttpClientError
{
public:
	HttpClientTimeoutError() : HttpClientError("HttpClient request timed out") { }
};

class HttpClientRequestError : public HttpClientError
{
public:
	HttpClientRequestError() : HttpClientError("HttpClient request failed") { }
};
