#pragma once

#include <WebCPP/Controls/ImageBox/ImageBox.h>

class PlayerButton : public ImageBox
{
public:
	PlayerButton(View* parent, std::string src, std::function<void()> onClick = {});
};
