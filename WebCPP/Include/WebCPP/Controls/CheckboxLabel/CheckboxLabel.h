#pragma once

#include "../../Core/View.h"

namespace web
{
	class Checkbox;
	class ImageBox;
	class TextLabel;

	class CheckboxLabel : public View
	{
	public:
		CheckboxLabel();
		CheckboxLabel(const std::string& text);

		void setChangeHandler(const std::function<void(bool checked)>& handler);

		void setEnabled(const bool value);
		bool getEnabled() const;

		void setChecked(const bool value);
		bool isChecked();

		void setSrc(const std::string& src);
		void setText(const std::string& text);

	private:
		std::shared_ptr<Checkbox> checkbox;
		std::shared_ptr<ImageBox> imagebox;
		std::shared_ptr<TextLabel> label;
	};
}
