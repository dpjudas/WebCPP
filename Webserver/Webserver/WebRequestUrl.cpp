
#include "Precomp.h"
#include "WebRequestUrl.h"

WebRequestUrl::WebRequestUrl(std::string initFullUrl) : fullUrl(std::move(initFullUrl))
{
	if (fullUrl.length() > 16 * 1024)
		throw std::runtime_error("URL is too long!");

	size_t pos = fullUrl.find_first_of(":/?");
	if (pos == std::string::npos)
		pos = fullUrl.length();

	if (pos < fullUrl.length() && fullUrl[pos] == ':')
	{
		scheme = fullUrl.substr(0, pos);
		pos++;
	}

	if (pos + 1 < fullUrl.length() && fullUrl[pos] == '/' && fullUrl[pos + 1] == '/')
	{
		pos += 2;

		size_t hostendpos = fullUrl.find_first_of("/?", pos);
		if (hostendpos == std::string::npos)
			hostendpos = fullUrl.length();

		host = fullUrl.substr(pos, hostendpos - pos);
		pos = hostendpos;
	}

	size_t queryPos = fullUrl.find('?', pos);
	if (queryPos != std::string::npos)
	{
		path = fullUrl.substr(pos, queryPos - pos);
		queryString = fullUrl.substr(queryPos);
	}
	else
	{
		path = fullUrl.substr(pos);
	}
}
