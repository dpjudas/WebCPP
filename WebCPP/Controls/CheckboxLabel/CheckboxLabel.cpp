#include "CheckboxLabel.h"
#include "../Checkbox/Checkbox.h"
#include "../ImageBox/ImageBox.h"
#include "../TextLabel/TextLabel.h"

CheckboxLabel::CheckboxLabel(View* parent) : HBoxView(parent)
{
	addClass("checkboxlabel");
	element->setStyle("align-items", "center");
	element->setStyle("margin", "2px 0");

	checkbox = new Checkbox(this);
}

void CheckboxLabel::setChangeHandler(const std::function<void(bool checked)>& handler)
{
	checkbox->setChangeHandler(handler);
}

void CheckboxLabel::setChecked(const bool value)
{
	const bool fireChangedEvent = (value != isChecked());
	checkbox->setChecked(value);
	if (fireChangedEvent == true)
		checkbox->element->dispatchEvent("change");
}

bool CheckboxLabel::isChecked()
{
	return checkbox->isChecked();
}

void CheckboxLabel::setEnabled(const bool value)
{
	checkbox->setEnabled(value);
}

bool CheckboxLabel::getEnabled() const
{
	return checkbox->getEnabled();
}

void CheckboxLabel::setSrc(const std::string& src)
{
	if (imagebox == nullptr)
	{
		imagebox = new ImageBox(this);
		imagebox->element->setStyle("padding-right", "2px");
		imagebox->setClickHandler([=]() { setChecked(!isChecked()); });
	}
	imagebox->setSrc(src);
	imagebox->setSize(16, 16);
}

void CheckboxLabel::setText(const std::string& text)
{
	if (label == nullptr)
	{
		label = new TextLabel(this);
		label->setClickHandler([=]() { setChecked(!isChecked()); });
	}
	label->setText(text);
}
