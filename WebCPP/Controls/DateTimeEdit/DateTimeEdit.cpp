
#include "DateTimeEdit.h"

DateTimeEdit::DateTimeEdit() : View("input")
{
	addClass("datetimeedit");
	element->setAttribute("type", "datetime-local");
}

void DateTimeEdit::setValue(const std::string& isodatetime)
{
	element->handle.set("value", isodatetime);
}

std::string DateTimeEdit::getValue()
{
	return element->handle["value"].as<std::string>();
}

void DateTimeEdit::setMin(const std::string& isodatetime)
{
	element->handle.set("min", isodatetime);
}

std::string DateTimeEdit::getMin()
{
	return element->handle["min"].as<std::string>();
}

void DateTimeEdit::setMax(const std::string& isodatetime)
{
	element->handle.set("max", isodatetime);
}

std::string DateTimeEdit::getMax()
{
	return element->handle["max"].as<std::string>();
}

void DateTimeEdit::setStep(const int seconds)
{
	element->handle.set("step", seconds);
}

int DateTimeEdit::getStep()
{
	return element->handle["step"].as<int>();
}

void DateTimeEdit::setEnabled(bool value)
{
	if (enabled != value)
	{
		if (value)
			element->removeAttribute("disabled");
		else
			element->setAttribute("disabled", "");
		enabled = value;
	}
}

bool DateTimeEdit::getEnabled() const
{
	return enabled;
}

bool DateTimeEdit::setFocus()
{
	element->focus();
	return true;
}
