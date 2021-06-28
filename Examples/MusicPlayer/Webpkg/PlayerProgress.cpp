
#include "PlayerProgress.h"
#include <WebCPP/Controls/SvgBox/SvgBox.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>

PlayerProgress::PlayerProgress(View* parent) : HBoxView(parent)
{
	progress = new SvgBox(this);
	progress->setExpanding();
	timeLabel = new TextLabel(this);

	addClass("playerprogress");
	progress->addClass("playerprogress-progress");
	timeLabel->addClass("playerprogress-timelabel");

	setProgress(0);
}

void PlayerProgress::setProgress(double value)
{
	Rect box = progress->element->getBoundingClientRect();
	progress->clear();
	progress->addElement(
		"rect",
		{
			{ "rx", "2" },
			{ "ry", "2" },
			{ "x", "0.5" },
			{ "y", "1" },
			{ "width", std::to_string(value * box.width) },
			{ "height", "4.5" },
			{ "fill", "#248cfb" }
		});
}
