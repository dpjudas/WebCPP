
#include "Checkbox.h"

Checkbox::Checkbox(View* parent) : View(parent, "input")
{
	addClass("checkbox");
	element->setAttribute("type", "checkbox");
}

void Checkbox::setValue(bool value)
{
	element->handle.set("checked", value);
}

bool Checkbox::getValue()
{
	return element->handle["checked"].as<bool>();
}

void Checkbox::setEnabled(bool value)
{
	if (enabled != value)
	{
		if (value)
		{
			element->removeAttribute("disabled");
		}
		else
		{
			element->setAttribute("disabled", "");
		}
		enabled = value;
	}
}

bool Checkbox::getEnabled() const
{
	return enabled;
}
