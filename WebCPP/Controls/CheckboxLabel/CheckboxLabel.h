#pragma once
#include "WebCPP/Core/View.h"

class Checkbox;
class ImageBox;
class TextLabel;

class CheckboxLabel : public HBoxView
{
public:
	CheckboxLabel(View* parent);

	void setChangeHandler(const std::function<void(bool checked)>& handler);

	// Checkbox
	void setChecked(const bool value);
	bool isChecked();
	void setEnabled(const bool value);
	bool getEnabled() const;

	// ImageBox
	void setSrc(const std::string& src);

	// TextLabel
	void setText(const std::string& text);

private:
	Checkbox* checkbox = nullptr;
	ImageBox* imagebox = nullptr;
	TextLabel* label = nullptr;

};
