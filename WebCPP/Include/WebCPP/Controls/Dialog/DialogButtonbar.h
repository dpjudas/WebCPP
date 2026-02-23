#pragma once

#include "../../Core/View.h"

namespace web
{
	class DialogButton;

	class DialogButtonbar : public View
	{
	public:
		DialogButtonbar();

		void setFlat(bool value);

		std::shared_ptr<DialogButton> addButton(std::string icon, std::string text, std::function<void()> callback, bool leftAlign = false);
		std::shared_ptr<DialogButton> addButton(std::string text, std::function<void()> callback, bool leftAlign = false) { return addButton({}, std::move(text), std::move(callback), leftAlign); }
		std::shared_ptr<View> addSeparator();

		std::shared_ptr<View> spacer;
	};
}
