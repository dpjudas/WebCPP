#pragma once

#include "../../Core/View.h"

namespace web
{
	class TextLabel : public View
	{
	public:
		TextLabel();
		TextLabel(const std::string& text);

		void setEnabled(bool value);
		bool getEnabled() const;

		void setCenterAlign();

		void setCenterVerticalAlign();

		void setText(std::string text);
		std::string getText();

		void setHtml(std::string html);
		std::string getHtml();

		void setFixedWidth(int width);
		void setFixedHeight(int height);

		void setColor(const std::string& color);
		void setBackgroundColor(const std::string& backgroundColor);

		void setBold(bool bold);

		std::function<void()> clicked;

	private:
		void onClicked(Event* event);

		bool enabled = true;
	};
}
