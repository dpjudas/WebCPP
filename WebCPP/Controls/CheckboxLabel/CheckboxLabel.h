#pragma once

#include "WebCPP/Core/View.h"

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
	Checkbox* checkbox = nullptr;
	ImageBox* imagebox = nullptr;
	TextLabel* label = nullptr;
};
