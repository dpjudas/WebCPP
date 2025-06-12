
#include "WebCPP/Controls/Toolbar/VToolbar.h"

VToolbar::VToolbar() : View("vtoolbar-view")
{
	auto layout = createVBoxLayout();
	layout->setGap(5.0);
}

VToolbarButton* VToolbar::addButton(std::string img, std::string text, std::function<void()> onClick)
{
	auto button = new VToolbarButton();
	button->addClass("vtoolbar-button");
	button->icon->setSrc(img);
	button->text->setText(text);
	if (onClick)
	{
		button->element->addEventListener("click", [=](Event* event) { onClick(); event->stopPropagation(); });
	}
	getLayout<VBoxLayout>()->addView(button);
	return button;
}
