#pragma once
#include "WebCPP/Core/View.h"

class ImageBox;
class RadioButton;
class TextLabel;

class RadioButtonLabel : public View
{
public:
	RadioButtonLabel();
	RadioButtonLabel(const std::string& text);
	RadioButtonLabel(const std::string& text, const std::string& name);

	void setChangeHandler(const std::function<void(bool checked)>& handler);

	void setEnabled(const bool value);
	bool getEnabled() const;

	// RadioButton
	RadioButton* getRadioButton() const { return radiobutton; }
	void setChecked(const bool value = true);
	bool isChecked();
	void setName(const std::string& value);
	void setValue(const std::string& value);

	// ImageBox
	void setSrc(const std::string& src);

	// TextLabel
	void setText(const std::string& text);

private:
	RadioButton* radiobutton = nullptr;
	ImageBox* imagebox = nullptr;
	TextLabel* label = nullptr;

};
