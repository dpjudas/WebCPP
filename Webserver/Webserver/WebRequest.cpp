
#include "Precomp.h"
#include "WebRequest.h"

std::string WebRequest::getHeader(const WebHeaderName& str) const
{
	auto it = headers.find(str);
	return it != headers.end() ? it->second : std::string();
}
