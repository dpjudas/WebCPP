
#include "WebCPP/Controls/Dialog/Dialog.h"
#include "WebCPP/Controls/Button/Button.h"
#include "WebCPP/Core/HtmlDocument.h"

namespace web
{
	Dialog::Dialog(const std::string& title) : View("dialog")
	{
		setupUi();
		setTitle(title);

		element->addEventListener("keydown", std::bind_front(&Dialog::onKeyDown, this));
		element->addEventListener("cancel", std::bind_front(&Dialog::onCancel, this));
	}

	void Dialog::setupUi()
	{
		header = std::make_shared<DialogHeader>();
		header->addClass("dialog-header");
		centerView = std::make_shared<View>();
		centerView->addClass("dialog-centerview");
		buttonbar = std::make_shared<DialogButtonbar>();
		buttonbar->addClass("dialog-buttonbar");
		element->addEventListener("click", std::bind_front(&Dialog::onClick, this));

		auto layout = createVBoxLayout();
		layout->addView(header);
		layout->addView(centerView, true, true);
		layout->addView(buttonbar);

		setSize(500, 250);
	}

	void Dialog::onClick(Event* event)
	{
		event->stopPropagation();
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
		element->setStyle("width", std::to_string(width) + "px");
		if (fixedHeight)
			element->setStyle("height", std::to_string(height) + "px");
		else
			element->setStyle("height", "auto");
	}

	void Dialog::onClose(Event* event)
	{
		reject();
	}

	void Dialog::onKeyDown(Event* event)
	{
		int keyCode = event->handle["keyCode"].as<int>();
		if (keyCode == 13 && acceptButton) // Enter
		{
			acceptButton->click(event);
			event->stopPropagation();
			event->preventDefault();
		}
	}

	void Dialog::onCancel(Event* event)
	{
		event->preventDefault(); // Prevent native close; we manage lifetime via done()
		if (cancelButton)
			cancelButton->click(event);
		else
			reject();
	}

	task<int> Dialog::exec()
	{
		HtmlDocument::body()->addView(shared_from_this());
		element->handle.call<void>("showModal");
		execTaskPromise = std::make_unique<task_promise<int>>();
		onModalAttach();
		if (!applyDefaultFocus() && !focusFirstChild())
		{
			element->setTabIndex(0);
			element->focus();
		}
		return execTaskPromise->get_future();
	}

	void Dialog::done(int resultCode)
	{
		auto pin = shared_from_this();
		element->handle.call<void>("close"); // close native top-layer if opened via exec()
		closeModal(); // detach + endModal() for ModalLayer if opened via showDialogModal()
		if (execTaskPromise)
		{
			execTaskPromise->set_value(resultCode);
			execTaskPromise.reset();
		}
	}
}
