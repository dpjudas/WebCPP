#pragma once

#include "WebPageBlock.h"

class ImageBox;
class TextLabel;

class BigTeaser : public WebPageBlock
{
public:
	BigTeaser(View* parent);

	void setImage(std::string src);
	void setSection(std::string text);
	void setHeadline(std::string text);

private:
	VBoxView* container = nullptr;
	ImageBox* image = nullptr;
	VBoxView* textblock = nullptr;
	TextLabel* section = nullptr;
	TextLabel* headline = nullptr;
};
