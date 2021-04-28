#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "DialogHeaderButton.h"

class TextLabel;
class DialogHeaderButton;

class WindowHeader : public HBoxView
{
public:
	WindowHeader(View* parent);

	TextLabel* caption = nullptr;
	DialogHeaderButton* minimizeButton = nullptr;
	DialogHeaderButton* maximizeButton = nullptr;
	DialogHeaderButton* closeButton = nullptr;

private:
	void onMinimize();
	void onMaximize();
	void onClose();
};
