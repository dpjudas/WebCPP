#pragma once

#include <WebCPP/Core/View.h>

class TextLabel;

class WebPageFooter : public VBoxView
{
public:
	WebPageFooter(View* parent);

private:
	TextLabel* copyright = nullptr;
};
