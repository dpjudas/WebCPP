#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "DialogHeaderButton.h"

class TextLabel;
class DialogHeaderButton;

class DialogHeader : public HBoxView
{
public:
	DialogHeader(View* parent);

	TextLabel* caption = nullptr;
	DialogHeaderButton* closeButton = nullptr;

	static const std::string CloseIconSvg;
	static const std::string MaximizedIconSvg;
	static const std::string MaximizeIconSvg;
	static const std::string MinimizeIconSvg;
};
