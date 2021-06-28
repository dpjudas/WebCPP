#pragma once

#include <WebCPP/Core/View.h>

class SvgBox;
class ImageBox;

class PlayerVolume : public HBoxView
{
public:
	PlayerVolume(View* parent);

	void setVolume(double value);

	ImageBox* minIcon = nullptr;
	SvgBox* volume = nullptr;
	ImageBox* maxIcon = nullptr;
};
