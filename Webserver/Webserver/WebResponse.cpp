
#include "Precomp.h"
#include "Webserver/WebResponse.h"

namespace web
{
	std::string WebResponse::getHeader(const WebHeaderName& str) const
	{
		auto it = headers.find(str);
		return it != headers.end() ? it->second : std::string();
	}
}
