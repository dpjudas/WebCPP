#pragma once

#include <WebCPP/Core/View.h>

class TextLabel;
class ImageBox;

class PodcastEpisode : public VBoxView
{
public:
	PodcastEpisode(View* parent);

private:
	TextLabel* title = nullptr;
	ImageBox* image = nullptr;
	TextLabel* description = nullptr;
};
