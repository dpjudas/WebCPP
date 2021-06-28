
#include "PlayerButton.h"

PlayerButton::PlayerButton(View* parent, std::string src, std::function<void()> onClick) : ImageBox(parent)
{
	addClass("playerbutton");
	setSrc(src);

	element->addEventListener("click", [=](Event* event) {
		event->stopPropagation();
		event->preventDefault();
		if (onClick)
			onClick();
	});
}
