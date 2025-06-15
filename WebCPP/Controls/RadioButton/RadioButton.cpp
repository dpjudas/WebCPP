
#include "WebCPP/Controls/RadioButton/RadioButton.h"

namespace web
{
	RadioButton::RadioButton() : View("input")
	{
		addClass("radiobutton");
		element->setAttribute("type", "radio");
		setChecked(false); // needed to force change event to fire when checked
	}

	void RadioButton::setChangeHandler(const std::function<void(bool checked)>& handler)
	{
		element->addEventListener("change", [=](Event* e) { e->stopPropagation(); handler(isChecked()); });
	}

	void RadioButton::setId(const std::string& value)
	{
		element->handle.set("id", value);
	}

	void RadioButton::setName(const std::string& value)
	{
		element->handle.set("name", value);
	}

	void RadioButton::setValue(const std::string& value)
	{
		element->handle.set("value", value);
	}

	std::string RadioButton::getValue() const
	{
		return element->handle["value"].as<std::string>();
	}

	void RadioButton::setChecked(bool value)
	{
		element->handle.set("checked", value);
	}

	bool RadioButton::isChecked() const
	{
		return element->handle["checked"].as<bool>();
	}

	void RadioButton::setEnabled(bool value)
	{
		element->handle.set("disabled", value == false);
	}

	bool RadioButton::getEnabled() const
	{
		return element->handle["disabled"].as<bool>() == false;
	}
}
