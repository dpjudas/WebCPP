
#include "WebCPP/Controls/TextLabel/TextLabel.h"

namespace web
{
	TextLabel::TextLabel() : View("textlabel-view")
	{
		element->addEventListener("click", [=](Event* e) { if (getEnabled() == true) onClicked(e); });
	}

	TextLabel::TextLabel(const std::string& text) : TextLabel()
	{
		setText(text);
	}

	void TextLabel::setEnabled(const bool value)
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
		const std::string& fixedHeight = element->getStyle("height");
		if (fixedHeight.empty() == false)
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

	void TextLabel::setFixedWidth(const int width)
	{
		element->setStyle("width", std::to_string(width) + "px");
	}

	void TextLabel::setFixedHeight(const int height)
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

	void TextLabel::setBold(const bool bold)
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
		if (clicked != nullptr)
		{
			event->stopPropagation();
			event->preventDefault();
			clicked();
		}
	}
}
