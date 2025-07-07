
#include "WebCPP/Controls/ImageBox/ImageBox.h"

namespace web
{
	ImageBox::ImageBox() : View("imagebox-view")
	{
		img = std::make_shared<View>("img");
		img->element->setStyle("visibility", "hidden");

		auto layout = createHBoxLayout();
		layout->addView(img);

		element->addEventListener("click", [=](Event* e) { if (getEnabled() == true) onClicked(e); });
	}

	void ImageBox::setEnabled(const bool value)
	{
		enabled = value;
	}

	bool ImageBox::getEnabled() const
	{
		return enabled;
	}

	void ImageBox::setSrc(std::string src)
	{
		if (!src.empty())
		{
			if (src.size() > 2 && src[0] == ':' && src[1] == '/')
			{
				src = resourceUrlBase + src.substr(2);
			}

			img->element->setAttribute("src", src);
			img->element->setStyle("visibility", "inherit");
		}
		else
		{
			img->element->removeAttribute("src");
			img->element->setStyle("visibility", "hidden");
		}
	}

	void ImageBox::setSize(const int width, const int height)
	{
		img->element->setStyle("width", std::to_string(width) + "px");
		img->element->setStyle("height", std::to_string(height) + "px");
	}

	void ImageBox::setAlt(const std::string& alternateText)
	{
		img->element->setAttribute("alt", alternateText);
	}

	void ImageBox::onClicked(Event* event)
	{
		if (clicked != nullptr)
		{
			event->stopPropagation();
			event->preventDefault();
			clicked();
		}
	}

	void ImageBox::setResourceUrlBase(const std::string& urlBase)
	{
		resourceUrlBase = urlBase;
	}

	std::string ImageBox::resourceUrlBase;
}
