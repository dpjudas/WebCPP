#pragma once

#include "WebCPP/Core/View.h"

class ImageBox : public View
{
public:
	ImageBox(View* parent);

	void setSrc(std::string src);
};
