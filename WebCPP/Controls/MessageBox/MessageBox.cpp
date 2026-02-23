
#include "WebCPP/Controls/MessageBox/MessageBox.h"
#include "WebCPP/Controls/Dialog/Dialog.h"
#include "WebCPP/Controls/Dialog/DialogButton.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	class MessageBoxView : public Dialog
	{
	public:
		MessageBoxView()
		{
			addClass("messagebox");
			auto centerView = getCenterView();
			labelText = std::make_shared<TextLabel>();
			buttonOK = getButtonBar()->addButton("OK", [=]() { onOK(); });
			buttonOK->element->setStyle("width", "80px");
			auto layout = centerView->createVBoxLayout();
			layout->addView(labelText);
		}

		void addCancelButton()
		{
			buttonCancel = getButtonBar()->addButton("Cancel", [=]() { onCancel(); });
			buttonCancel->element->setStyle("width", "80px");
		}

		void onOK()
		{
			auto cb = okCallback;
			closeModal();
			if (cb)
				cb();
		}

		void onCancel()
		{
			auto cb = cancelCallback;
			closeModal();
			if (cb)
				cb();
		}

		void onClose(Event* event) override
		{
			auto cb = cancelCallback;
			Dialog::onClose(event);
			if (cb)
				cb();
		}

		std::shared_ptr<TextLabel> labelText;
		std::shared_ptr<DialogButton> buttonOK;
		std::shared_ptr<DialogButton> buttonCancel;
		std::function<void()> okCallback, cancelCallback;
	};

	void MessageBox::show(const std::string& title, const std::string& text, std::function<void()> okClicked, const std::string& okLabel)
	{
		auto dialog = std::make_shared<MessageBoxView>();
		dialog->setTitle(title);
		dialog->labelText->setText(text);
		dialog->buttonOK->setText(okLabel);
		dialog->okCallback = okClicked;
		dialog->cancelCallback = okClicked;
		dialog->showDialogModal();
	}

	void MessageBox::question(const std::string& title, const std::string& text, std::function<void()> okClicked, std::function<void()> cancelClicked, const std::string& okLabel, const std::string& cancelLabel)
	{
		auto dialog = std::make_shared<MessageBoxView>();
		dialog->addCancelButton();
		dialog->setTitle(title);
		dialog->labelText->setText(text);
		dialog->buttonOK->setText(okLabel);
		dialog->buttonCancel->setText(cancelLabel);
		dialog->okCallback = okClicked;
		dialog->cancelCallback = cancelClicked;
		dialog->showDialogModal();
	}
}
