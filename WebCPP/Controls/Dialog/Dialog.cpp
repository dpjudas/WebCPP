
#include "WebCPP/Controls/Dialog/Dialog.h"
#include "WebCPP/Controls/Button/Button.h"
#include <cmath>

namespace web
{
	Dialog::Dialog(const std::string& title) : View("dialog-view")
	{
		setupUi();
		setTitle(title);

		element->addEventListener("keydown", [=](Event* e) { onKeyDown(e); });
	}

	void Dialog::setupUi()
	{
		header = std::make_shared<DialogHeader>();
		header->addClass("dialog-header");
		centerView = std::make_shared<View>();
		centerView->addClass("dialog-centerview");
		buttonbar = std::make_shared<DialogButtonbar>();
		buttonbar->addClass("dialog-buttonbar");
		element->addEventListener("click", [](Event* event) { event->stopPropagation(); });

		auto layout = createVBoxLayout();
		layout->addView(header);
		layout->addView(centerView, true, true);
		layout->addView(buttonbar);

		setSize(500, 250);
	}

	void Dialog::setCenterView(std::shared_ptr<View> view)
	{
		if (!view)
			view = std::make_shared<View>();

		centerView->removeClass("dialog-centerview");
		centerView = view;
		centerView->addClass("dialog-centerview");

		auto layout = getLayout<VBoxLayout>();
		layout->clear();
		layout->addView(header);
		layout->addView(centerView, true, true);
		layout->addView(buttonbar);
	}

	void Dialog::setTitle(std::string text)
	{
		header->caption->setText(text);
	}

	void Dialog::setSize(double width, double height, bool fixedHeight)
	{
		element->setStyle("left", "calc(50vw - " + std::to_string(std::round(width * 0.5)) + "px)");
		element->setStyle("top", "calc(50vh - " + std::to_string(std::round(height * 0.5)) + "px)");
		element->setStyle("width", std::to_string(width) + "px");
		if (fixedHeight)
			element->setStyle("height", std::to_string(height) + "px");
		else
			element->setStyle("height", "auto");
	}

	void Dialog::onClose(Event* event)
	{
		closeModal();
	}

	void Dialog::onKeyDown(Event* event)
	{
		int keyCode = event->handle["keyCode"].as<int>();
		if (keyCode == 13) // Enter
		{
			if (acceptButton)
				acceptButton->click(event);
		}
		else if (keyCode == 27) // Escape
		{
			if (cancelButton)
				cancelButton->click(event);
		}
	}
}
