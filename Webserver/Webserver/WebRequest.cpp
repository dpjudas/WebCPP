
#include "Precomp.h"
#include "Webserver/WebRequest.h"

namespace web
{
	std::string WebRequest::getHeader(const WebHeaderName& str) const
	{
		auto it = headers.find(str);
		return it != headers.end() ? it->second : std::string();
	}
}
