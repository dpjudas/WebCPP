#pragma once

#include "../../Core/View.h"

namespace web
{
	class ImageBox;

	class LineEdit : public View
	{
	public:
		LineEdit();

		ImageBox* addButton(const std::string& icon, const std::string& text, const std::function<void()>& clickHandler);

		bool getPasswordMode() const;
		void setPasswordMode(bool value);
		void setText(std::string text);
		void setPlaceholder(std::string text);
		std::string getText();
		void setEnabled(bool value);
		bool getEnabled() const;
		void setReadOnly(const bool value);
		bool getReadOnly() const;
		void setMaxLength(const int value);

		void setFlat();

		bool setFocus() override;
		bool setFocusWithoutSelect();

		void setInputHandler(const std::function<void(const std::string& text)>& handler);
		void setChangeHandler(const std::function<void(const std::string& text)>& handler);

	private:
		bool enabled = true;
		bool readonly = false;
		View* input = nullptr;
	};
}
