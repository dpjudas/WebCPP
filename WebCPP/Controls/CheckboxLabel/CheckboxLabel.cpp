
#include "WebCPP/Controls/CheckboxLabel/CheckboxLabel.h"
#include "WebCPP/Controls/Checkbox/Checkbox.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	CheckboxLabel::CheckboxLabel() : View("checkboxlabel-view")
	{
		checkbox = std::make_shared<Checkbox>();

		auto layout = createHBoxLayout();
		layout->setAlignItems(FlexAlignItems::center);
		layout->setGap(2.0);
		layout->addView(checkbox);
	}

	CheckboxLabel::CheckboxLabel(const std::string& text) : CheckboxLabel()
	{
		setText(text);
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
		imagebox->setEnabled(value);
		label->setEnabled(value);
	}

	bool CheckboxLabel::getEnabled() const
	{
		return checkbox->getEnabled();
	}

	void CheckboxLabel::setSrc(const std::string& src)
	{
		if (!imagebox)
		{
			imagebox = std::make_shared<ImageBox>();
			imagebox->element->setStyle("padding-right", "2px");
			imagebox->clicked = [=]() { if (getEnabled() == true) setChecked(!isChecked()); };

			getLayout<HBoxLayout>()->addView(imagebox);
		}
		imagebox->setSrc(src);
		imagebox->setSize(16, 16);
	}

	void CheckboxLabel::setText(const std::string& text)
	{
		if (label == nullptr)
		{
			label = std::make_shared<TextLabel>();
			label->clicked = [=]() { if (getEnabled() == true) setChecked(!isChecked()); };

			getLayout<HBoxLayout>()->addView(label);
		}
		label->setText(text);
	}
}
