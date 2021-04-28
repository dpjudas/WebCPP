
#include "ImageBox.h"

ImageBox::ImageBox(View* parent) : View(parent, "img")
{
	addClass("imagebox");
	element->setStyle("visibility", "hidden");
}

void ImageBox::setSrc(std::string src)
{
	if (!src.empty())
	{
		element->setAttribute("src", src);
		element->setStyle("visibility", "inherit");
	}
	else
	{
		element->removeAttribute("src");
		element->setStyle("visibility", "hidden");
	}
}
