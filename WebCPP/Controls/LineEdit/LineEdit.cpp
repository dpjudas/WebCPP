
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"

namespace web
{
	LineEdit::LineEdit() : View("lineedit-view")
	{
		input = std::make_shared<View>("input");
		input->element->setAttribute("type", "text");
		input->element->addEventListener("focus", std::bind_front(&LineEdit::onFocus, this));
		input->element->addEventListener("blur", std::bind_front(&LineEdit::onBlur, this));

		auto layout = createHBoxLayout();
		layout->addView(input, true, true);
	}

	void LineEdit::onFocus(Event* event)
	{
		addClass("focus");
	}

	void LineEdit::onBlur(Event* event)
	{
		removeClass("focus");
	}

	std::shared_ptr<ImageBox> LineEdit::addButton(const std::string& icon, const std::string& text, const std::function<void()>& clickHandler)
	{
		auto result = std::make_shared<ImageBox>();
		result->addClass("lineedit-imagebox");
		result->setAlt(text);
		result->setSrc(icon);
		result->clicked = clickHandler;
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

	void LineEdit::setReadOnly(bool value)
	{
		if (readonly != value)
		{
			if (!value)
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

	void LineEdit::setMaxLength(int value)
	{
		input->element->setAttribute("maxlength", std::to_string(value));
	}

	void LineEdit::setFlat()
	{
		addClass("flat");
	}

	bool LineEdit::setFocus()
	{
		if (getEnabled())
		{
			input->element->focus();
			if (!getReadOnly())
				input->element->handle.call<void>("select");
			return true;
		}
		return false;
	}

	bool LineEdit::setFocusWithoutSelect()
	{
		if (getEnabled())
		{
			input->element->focus();
			return true;
		}
		return false;
	}

	void LineEdit::setInputHandler(const std::function<void(const std::string& text)>& handler)
	{
		input->element->addEventListener("input", std::bind_front(&LineEdit::onInput, this, handler));
	}

	void LineEdit::setChangeHandler(const std::function<void(const std::string& text)>& handler)
	{
		input->element->addEventListener("change", std::bind_front(&LineEdit::onChange, this, handler));
	}

	void LineEdit::onInput(std::function<void(const std::string& text)> handler, Event* event)
	{
		event->stopPropagation();
		handler(getText());
	}

	void LineEdit::onChange(std::function<void(const std::string& text)> handler, Event* event)
	{
		event->stopPropagation();
		handler(getText());
	}
}
