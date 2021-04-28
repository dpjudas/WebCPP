#pragma once

#include "WebCPP/Core/View.h"

class TextLabel;
class StatusbarItem;

class Statusbar : public HBoxView
{
public:
	Statusbar(View* parent);

	StatusbarItem* addItem(std::string text);
};

class StatusbarItem : public HBoxView
{
public:
	StatusbarItem(View* parent);

	TextLabel* text = nullptr;
};
