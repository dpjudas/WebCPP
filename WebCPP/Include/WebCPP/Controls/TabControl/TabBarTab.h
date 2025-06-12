#pragma once

#include "../../Core/View.h"

class ImageBox;
class TextLabel;

class TabBarTab : public View
{
public:
	TabBarTab();

	void setText(const std::string& text);
	void setIcon(const std::string& src);

private:
	ImageBox* icon = nullptr;
	TextLabel* label = nullptr;
};
