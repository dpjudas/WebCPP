
#include "ThreeChoices.h"
#include <WebCPP/Controls/ImageBox/ImageBox.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>

ThreeChoices::ThreeChoices(View* parent) : WebPageBlock(parent)
{
	list = new HBoxView(this);
	left = new ThreeChoicesTeaser(list);
	center = new ThreeChoicesTeaser(list);
	right = new ThreeChoicesTeaser(list);

	addClass("threechoices");
	list->addClass("threechoices-list");
	left->addClass("threechoices-left");
	center->addClass("threechoices-center");
	right->addClass("threechoices-right");
}

/////////////////////////////////////////////////////////////////////////////

ThreeChoicesTeaser::ThreeChoicesTeaser(View* parent) : VBoxView(parent)
{
	image = new ImageBox(this);
	textblock = new VBoxView(this);
	section = new TextLabel(textblock);
	headline = new TextLabel(textblock);

	addClass("threechoicesteaser");
	image->addClass("threechoicesteaser-image");
	textblock->addClass("threechoicesteaser-textblock");
	section->addClass("threechoicesteaser-section");
	headline->addClass("threechoicesteaser-headline");
}

void ThreeChoicesTeaser::setImage(std::string src)
{
	image->setSrc(src);
}

void ThreeChoicesTeaser::setSection(std::string text)
{
	section->setText(text);
}

void ThreeChoicesTeaser::setHeadline(std::string text)
{
	headline->setText(text);
}
