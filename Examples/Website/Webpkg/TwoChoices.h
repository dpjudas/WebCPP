#pragma once

#include "WebPageBlock.h"

class TwoChoicesTeaser;
class ImageBox;
class TextLabel;

class TwoChoices : public WebPageBlock
{
public:
	TwoChoices(View* parent);

	TextLabel* headline = nullptr;
	TwoChoicesTeaser* left = nullptr;
	TwoChoicesTeaser* right = nullptr;

private:
	HBoxView* list = nullptr;
};

class TwoChoicesTeaser : public VBoxView
{
public:
	TwoChoicesTeaser(View* parent);

	void setImage(std::string src);
	void setSection(std::string text);
	void setHeadline(std::string text);

private:
	ImageBox* image = nullptr;
	VBoxView* textblock = nullptr;
	TextLabel* section = nullptr;
	TextLabel* headline = nullptr;
};
