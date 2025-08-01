
#include "WebCPP/Controls/Button/Button.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	Button::Button() : View("button-view")
	{
		button = std::make_shared<View>("button");
		label = std::make_shared<TextLabel>();

		auto buttonLayout = button->createHBoxLayout();
		buttonLayout->addView(label, true, false);

		button->element->addEventListener("click", [=](Event* e) { onClicked(e); });
		button->element->addEventListener("keydown", [=](Event* e) { onKeyDown(e); });

		auto layout = createHBoxLayout();
		layout->addView(button, true, false);
	}

	void Button::setIcon(std::string src)
	{
		if (!image)
		{
			image = std::make_shared<ImageBox>();
			button->getLayout<HBoxLayout>()->addViewBefore(image, label);
		}
		image->setSrc(src);
	}

	std::string Button::getText() const
	{
		return label->getText();
	}

	void Button::setText(std::string text)
	{
		label->setText(text);
	}

	void Button::setEnabled(bool value)
	{
		if (enabled != value)
		{
			if (value)
				button->element->removeAttribute("disabled");
			else
				button->element->setAttribute("disabled", "");
			enabled = value;
		}
	}

	bool Button::getEnabled() const
	{
		return enabled;
	}

	bool Button::setFocus()
	{
		button->element->focus();
		return true;
	}

	void Button::click(Event* event)
	{
		onClicked(event);
	}

	void Button::onClicked(Event* event)
	{
		event->stopPropagation();
		event->preventDefault();
		if (pressed)
			pressed();
	}

	void Button::onKeyDown(Event* event)
	{
		int keyCode = event->handle["keyCode"].as<int>();
		if (keyCode == 13 || keyCode == 32) // Enter or space
		{
			event->stopPropagation();
		}
	}
}
