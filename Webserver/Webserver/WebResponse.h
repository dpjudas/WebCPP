#pragma once

#include "WebHeaderName.h"
#include "Common/IOData/DataBuffer.h"

class WebResponse
{
public:
	struct
	{
		int code = 0;
		std::string text;
	} status;
	std::map<WebHeaderName, std::string> headers;
	std::shared_ptr<DataBuffer> content;

	std::string getHeader(const WebHeaderName& str) const;
};
