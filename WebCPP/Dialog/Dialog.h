#pragma once

#include "WebCPP/Core/View.h"
#include "DialogHeader.h"
#include "DialogButtonbar.h"
#include <cmath>

class Dialog : public VBoxView
{
public:
	Dialog();

	void setTitle(std::string text);
	void setSize(double width, double height, bool fixedHeight = false);

	virtual void onClose(Event* event);

	DialogHeader* header = nullptr;
	VBoxView* centerView = nullptr;
	DialogButtonbar* buttonbar = nullptr;
};
