
#include "PlayerVolume.h"
#include <WebCPP/Controls/SvgBox/SvgBox.h>
#include <WebCPP/Controls/ImageBox/ImageBox.h>

PlayerVolume::PlayerVolume(View* parent) : HBoxView(parent)
{
	minIcon = new ImageBox(this);
	volume = new SvgBox(this);
	maxIcon = new ImageBox(this);

	minIcon->setSrc("/Resources/Icons/volume_mute.svg");
	maxIcon->setSrc("/Resources/Icons/volume_up.svg");

	addClass("playerprogress");
	minIcon->addClass("playervolume-icon");
	volume->addClass("playervolume-volume");
	maxIcon->addClass("playervolume-icon");

	setVolume(1);
}

void PlayerVolume::setVolume(double value)
{
	volume->clear();
	volume->addElement(
		"rect",
		{
			{ "rx", "2" },
			{ "ry", "2" },
			{ "x", "0.5" },
			{ "y", "1" },
			{ "width", std::to_string(value * 150) },
			{ "height", "4.5" },
			{ "fill", "#248cfb" }
		});
}
