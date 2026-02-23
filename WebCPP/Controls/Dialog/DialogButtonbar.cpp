
#include "WebCPP/Controls/Dialog/DialogButtonbar.h"
#include "WebCPP/Controls/Dialog/DialogButton.h"

namespace web
{
	DialogButtonbar::DialogButtonbar() : View("dialogbuttonbar-view")
	{
		spacer = std::make_shared<View>();
		spacer->addClass("dialogbuttonbar-spacer");

		auto layout = createHBoxLayout();
		layout->addView(spacer, true, true);
	}

	void DialogButtonbar::setFlat(bool value)
	{
		if (value)
			addClass("flat");
		else
			removeClass("flat");
	}

	std::shared_ptr<DialogButton> DialogButtonbar::addButton(std::string icon, std::string text, std::function<void()> callback, bool leftAlign)
	{
		auto button = std::make_shared<DialogButton>();
		button->addClass("dialogbuttonbar-button");
		if (!icon.empty())
			button->setIcon(icon);
		button->setText(text);
		button->pressed = callback;

		getLayout<HBoxLayout>()->addViewBefore(button, leftAlign ? spacer : nullptr);

		return button;
	}

	std::shared_ptr<View> DialogButtonbar::addSeparator()
	{
		auto sep = std::make_shared<View>();
		sep->addClass("dialogbuttonbar-sep");
		getLayout<HBoxLayout>()->addView(sep);
		return sep;
	}
}
