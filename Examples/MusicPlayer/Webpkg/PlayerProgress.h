#pragma once

#include <WebCPP/Core/View.h>

class SvgBox;
class TextLabel;

class PlayerProgress : public HBoxView
{
public:
	PlayerProgress(View* parent);

	void setProgress(double value);

	SvgBox* progress = nullptr;
	TextLabel* timeLabel = nullptr;
};
