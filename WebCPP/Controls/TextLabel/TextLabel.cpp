
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	TextLabel::TextLabel() : View("textlabel-view")
	{
		element->addEventListener("click", std::bind_front(&TextLabel::onClicked, this));
	}

	TextLabel::TextLabel(const std::string& text) : TextLabel()
	{
		setText(text);
	}

	void TextLabel::setEnabled(bool value)
	{
		enabled = value;

		element->setStyle("color", enabled ? "#000000" : "#888888");
	}

	bool TextLabel::getEnabled() const
	{
		return enabled;
	}

	void TextLabel::setCenterAlign()
	{
		element->setStyle("textAlign", "center");
	}

	void TextLabel::setCenterVerticalAlign()
	{
		std::string fixedHeight = element->getStyle("height");
		if (!fixedHeight.empty())
			element->setStyle("line-height", fixedHeight);
	}

	void TextLabel::setText(std::string text)
	{
		element->handle.set("innerText", text);
	}

	std::string TextLabel::getText()
	{
		return element->handle["innerText"].as<std::string>();
	}

	void TextLabel::setHtml(std::string html)
	{
		element->handle.set("innerHTML", html);
	}

	std::string TextLabel::getHtml()
	{
		return element->handle["innerHTML"].as<std::string>();
	}

	void TextLabel::setFixedWidth(int width)
	{
		element->setStyle("width", std::to_string(width) + "px");
	}

	void TextLabel::setFixedHeight(int height)
	{
		element->setStyle("height", std::to_string(height) + "px");
		if (element->getStyle("line-height").empty() == false) // if centered vertically, apply same height
			setCenterVerticalAlign();
	}

	void TextLabel::setColor(const std::string& color)
	{
		element->setStyle("color", color);
	}

	void TextLabel::setBackgroundColor(const std::string& backgroundColor)
	{
		element->setStyle("background-color", backgroundColor);
	}

	void TextLabel::setBold(bool bold)
	{
		/*
		100 – Thin
		200 – Extra Light (Ultra Light)
		300 – Light
		400 – Normal
		500 – Medium
		600 – Semi Bold (Demi Bold)
		700 – Bold
		800 – Extra Bold (Ultra Bold)
		900 – Black (Heavy)
		*/
		element->setStyle("font-weight", bold ? "bold" : "normal");
	}

	void TextLabel::onClicked(Event* event)
	{
		if (!getEnabled())
			return;

		if (clicked)
		{
			event->stopPropagation();
			event->preventDefault();
			clicked();
		}
	}
}
