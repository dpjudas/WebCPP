#pragma once

#include "../../Core/View.h"

class TextLabel : public View
{
public:
	TextLabel();
	TextLabel(const std::string& text);

	void setEnabled(const bool value);
	bool getEnabled() const;

	void setCenterAlign();

	void setCenterVerticalAlign();

	void setText(std::string text);
	std::string getText();

	void setHtml(std::string html);
	std::string getHtml();

	void setFixedWidth(const int width);
	void setFixedHeight(const int height);

	void setColor(const std::string& color);
	void setBackgroundColor(const std::string& backgroundColor);

	void setBold(const bool bold);

	std::function<void()> clicked;

private:
	void onClicked(Event* event);

private:
	bool enabled = true;

};
