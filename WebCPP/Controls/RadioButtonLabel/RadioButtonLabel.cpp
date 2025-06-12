
#include "WebCPP/Controls/RadioButtonLabel/RadioButtonLabel.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/RadioButton/RadioButton.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

RadioButtonLabel::RadioButtonLabel() : View("radiobuttonlabel-view")
{
	radiobutton = new RadioButton();

	auto layout = createHBoxLayout();
	layout->setAlignItems(FlexAlignItems::center);
	layout->setGap(2.0);
	layout->addView(radiobutton);
}

RadioButtonLabel::RadioButtonLabel(const std::string& text) : RadioButtonLabel()
{
	setText(text);
}

RadioButtonLabel::RadioButtonLabel(const std::string& text, const std::string& name) : RadioButtonLabel()
{
	radiobutton->setName(name);
	setText(text);
}

void RadioButtonLabel::setChangeHandler(const std::function<void(bool checked)>& handler)
{
	radiobutton->setChangeHandler(handler);
}

void RadioButtonLabel::setChecked(const bool value)
{
	const bool fireChangedEvent = (value != isChecked());
	radiobutton->setChecked(value);
	if (fireChangedEvent == true)
		radiobutton->element->dispatchEvent("change");
}

bool RadioButtonLabel::isChecked()
{
	return radiobutton->isChecked();
}

void RadioButtonLabel::setName(const std::string& value)
{
	radiobutton->setName(value);
}

void RadioButtonLabel::setValue(const std::string& value)
{
	radiobutton->setValue(value);
}

void RadioButtonLabel::setEnabled(const bool value)
{
	radiobutton->setEnabled(value);
	imagebox->setEnabled(value);
	label->setEnabled(value);
}

bool RadioButtonLabel::getEnabled() const
{
	return radiobutton->getEnabled();
}

void RadioButtonLabel::setSrc(const std::string& src)
{
	if (imagebox == nullptr)
	{
		imagebox = new ImageBox();
		imagebox->element->setStyle("padding-right", "2px");
		imagebox->clicked = [=]() { if (getEnabled() == true) setChecked(!isChecked()); };

		getLayout<HBoxLayout>()->addView(imagebox);
	}
	imagebox->setSrc(src);
	imagebox->setSize(16, 16);
}

void RadioButtonLabel::setText(const std::string& text)
{
	if (label == nullptr)
	{
		label = new TextLabel();
		label->clicked = [=]() { if (getEnabled() == true) setChecked(!isChecked()); };

		getLayout<HBoxLayout>()->addView(label);
	}
	label->setText(text);
}
