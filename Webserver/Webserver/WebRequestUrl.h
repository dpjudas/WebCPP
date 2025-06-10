#pragma once

class WebRequestUrl
{
public:
	WebRequestUrl() = default;
	WebRequestUrl(std::string fullUrl);

	// Returns the entire url: "http://hostname:port/abs/.../path?query"
	const std::string& getFullUrl() const { return fullUrl; }

	// Returns the url scheme: "http"
	const std::string& getScheme() const { return scheme; }

	// Returns the url hostname and port: "hostname:port"
	const std::string& getHost() const { return host; }

	// Returns the absolute path: "/abs/.../path"
	const std::string& getPath() const { return path; }

	// Returns the query string: "?query"
	const std::string& getQueryString() const { return queryString; }

private:
	std::string fullUrl;
	std::string scheme;
	std::string host;
	std::string path;
	std::string queryString;
};
