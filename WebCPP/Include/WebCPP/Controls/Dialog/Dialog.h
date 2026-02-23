#pragma once

#include "../../Core/View.h"
#include "DialogHeader.h"
#include "DialogButtonbar.h"

namespace web
{
	class Button;

	class Dialog : public View
	{
	public:
		Dialog(const std::string& title = std::string());

		void setTitle(std::string text);
		void setSize(double width, double height, bool fixedHeight = false);

		void setCenterView(std::shared_ptr<View> view);

		std::shared_ptr<DialogHeader> getHeader() { return header; }
		std::shared_ptr<View> getCenterView() { return centerView; }
		std::shared_ptr<DialogButtonbar> getButtonBar() { return buttonbar; }

		virtual void onClose(Event* event);

		void setAcceptButton(std::shared_ptr<Button> button) { acceptButton = button; }
		void setCancelButton(std::shared_ptr<Button> button) { cancelButton = button; }

	private:
		void setupUi();
		void onKeyDown(Event* event);

		std::shared_ptr<DialogHeader> header;
		std::shared_ptr<View> centerView;
		std::shared_ptr<DialogButtonbar> buttonbar;

	private:
		std::shared_ptr<Button> acceptButton;
		std::shared_ptr<Button> cancelButton;
	};
}
