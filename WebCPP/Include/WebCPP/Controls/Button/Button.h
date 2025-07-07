#pragma once

#include "../../Core/View.h"

namespace web
{
	class ImageBox;
	class TextLabel;

	class Button : public View
	{
	public:
		Button();

		void setIcon(std::string src);
		std::string getText() const;
		void setText(std::string text);
		void setEnabled(bool value);
		bool getEnabled() const;

		bool enabled = true;

		bool setFocus() override;

		void click(Event* event);

		std::function<void()> pressed;

	private:
		void onClicked(Event* event);
		void onKeyDown(Event* event);

		std::shared_ptr<View> button;
		std::shared_ptr<ImageBox> image;
		std::shared_ptr<TextLabel> label;
	};
}
