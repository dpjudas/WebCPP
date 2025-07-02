#pragma once

#include "../../Core/View.h"

namespace web
{
	class ImageBox;
	class TextLabel;

	class TabBarTab : public View
	{
	public:
		TabBarTab();

		void setText(const std::string& text);
		void setIcon(const std::string& src);

	private:
		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> label;
	};
}
