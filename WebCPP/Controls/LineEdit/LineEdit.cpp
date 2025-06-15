
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"

namespace web
{
	LineEdit::LineEdit() : View("lineedit-view")
	{
		input = new View("input");
		input->element->setAttribute("type", "text");
		input->element->addEventListener("focus", [=](Event* event) { addClass("focus"); });
		input->element->addEventListener("blur", [=](Event* event) { removeClass("focus"); });

		auto layout = createHBoxLayout();
		layout->addView(input, true, true);
	}

	ImageBox* LineEdit::addButton(const std::string& icon, const std::string& text, const std::function<void()>& clickHandler)
	{
		ImageBox* result = new ImageBox();
		result->addClass("lineedit-imagebox");
		result->setAlt(text);
		result->setSrc(icon);
		result->clicked = clickHandler;
		result->setEnabled(clickHandler != nullptr);
		getLayout<HBoxLayout>()->addView(result);
		return result;
	}

	bool LineEdit::getPasswordMode() const
	{
		return input->element->getAttribute("type") == "password";
	}

	void LineEdit::setPasswordMode(bool value)
	{
		input->element->setAttribute("type", value ? "password" : "text");
	}

	void LineEdit::setText(std::string text)
	{
		input->element->handle.set("value", text);
	}

	void LineEdit::setPlaceholder(std::string text)
	{
		input->element->handle.set("placeholder", text);
	}

	std::string LineEdit::getText()
	{
		return input->element->handle["value"].as<std::string>();
	}

	void LineEdit::setEnabled(bool value)
	{
		if (enabled != value)
		{
			if (value)
			{
				input->element->removeAttribute("disabled");
				removeClass("disabled");
			}
			else
			{
				input->element->setAttribute("disabled", "");
				addClass("disabled");
			}
			enabled = value;
		}
	}

	bool LineEdit::getEnabled() const
	{
		return enabled;
	}

	void LineEdit::setReadOnly(const bool value)
	{
		if (readonly != value)
		{
			if (value == false)
			{
				input->element->removeAttribute("readonly");
				removeClass("readonly");
			}
			else
			{
				input->element->setAttribute("readonly", "");
				addClass("readonly");
			}
			readonly = value;
		}
	}

	bool LineEdit::getReadOnly() const
	{
		return readonly;
	}

	void LineEdit::setMaxLength(const int value)
	{
		input->element->setAttribute("maxlength", std::to_string(value));
	}

	void LineEdit::setFlat()
	{
		addClass("flat");
	}

	bool LineEdit::setFocus()
	{
		if (getEnabled() == true)
		{
			input->element->focus();
			if (getReadOnly() == false)
				input->element->handle.call<void>("select");
			return true;
		}
		return false;
	}

	bool LineEdit::setFocusWithoutSelect()
	{
		if (getEnabled() == true)
		{
			input->element->focus();
			return true;
		}
		return false;
	}

	void LineEdit::setInputHandler(const std::function<void(const std::string& text)>& handler)
	{
		input->element->addEventListener("input", [=](Event* e) { e->stopPropagation(); handler(getText()); });
	}

	void LineEdit::setChangeHandler(const std::function<void(const std::string& text)>& handler)
	{
		input->element->addEventListener("change", [=](Event* e) { e->stopPropagation(); handler(getText()); });
	}
}
