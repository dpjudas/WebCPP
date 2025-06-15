#pragma once

#include "DataBuffer.h"
#include "WebRequestUrl.h"
#include "WebRequestVerb.h"
#include "WebHeaderName.h"
#include <memory>
#include <string>
#include <map>

namespace web
{
	class WebRequest
	{
	public:
		WebRequestVerb verb = WebRequestVerb::get;
		WebRequestUrl url;
		std::map<WebHeaderName, std::string> headers;
		std::shared_ptr<DataBuffer> content;

		std::string getHeader(const WebHeaderName& str) const;
	};
}
