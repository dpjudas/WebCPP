#pragma once

#include "WebPageBlock.h"

class ThreeChoicesTeaser;
class ImageBox;
class TextLabel;

class ThreeChoices : public WebPageBlock
{
public:
	ThreeChoices(View* parent);

	ThreeChoicesTeaser* left = nullptr;
	ThreeChoicesTeaser* center = nullptr;
	ThreeChoicesTeaser* right = nullptr;

private:
	HBoxView* list = nullptr;
};

class ThreeChoicesTeaser : public VBoxView
{
public:
	ThreeChoicesTeaser(View* parent);

	void setImage(std::string src);
	void setSection(std::string text);
	void setHeadline(std::string text);

private:
	ImageBox* image = nullptr;
	VBoxView* textblock = nullptr;
	TextLabel* section = nullptr;
	TextLabel* headline = nullptr;
};
