
#include "WebCPP/Controls/Checkbox/Checkbox.h"

namespace web
{
	Checkbox::Checkbox() : View("input")
	{
		addClass("checkbox");
		element->setAttribute("type", "checkbox");
	}

	void Checkbox::setChangeHandler(const std::function<void(bool checked)>& handler)
	{
		element->addEventListener("change", std::bind_front(&Checkbox::onChange, this, handler));
	}

	void Checkbox::setChecked(bool value)
	{
		element->handle.set("checked", value);
	}

	bool Checkbox::isChecked() const
	{
		return element->handle["checked"].as<bool>();
	}

	void Checkbox::setEnabled(bool value)
	{
		element->handle.set("disabled", value == false);
	}

	bool Checkbox::getEnabled() const
	{
		return element->handle["disabled"].as<bool>() == false;
	}

	void Checkbox::onChange(std::function<void(bool checked)> handler, Event* e)
	{
		e->stopPropagation();
		handler(isChecked());
	}
}
