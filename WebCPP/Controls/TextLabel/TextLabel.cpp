
#include "TextLabel.h"

TextLabel::TextLabel(View* parent) : View(parent)
{
	addClass("textlabel");
}

void TextLabel::setCenterAlign()
{
	element->setStyle("textAlign", "center");
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

void TextLabel::setClickHandler(const std::function<void()>& handler)
{
	element->addEventListener("click", [=](Event* e) { e->stopPropagation(); handler(); });
}
