
#include "WebCPP/Controls/IconBox/IconBox.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"

namespace web
{
	IconBox::Matcher IconBox::extendedMatcher;
	IconBox::Factory IconBox::extendedFactory;

	void IconBox::setExtendedFactory(Matcher matches, Factory create)
	{
		extendedMatcher = std::move(matches);
		extendedFactory = std::move(create);
	}

	bool IconBox::isExtendedSrc(const std::string& src)
	{
		return extendedMatcher != nullptr && extendedMatcher(src);
	}

	std::shared_ptr<IconBox> IconBox::createExtended()
	{
		return extendedFactory != nullptr ? extendedFactory() : nullptr;
	}

	std::shared_ptr<IconBox> createIconBox(const std::string& src)
	{
		if (IconBox::isExtendedSrc(src))
		{
			if (auto box = IconBox::createExtended())
				return box;
		}
		return std::make_shared<ImageBox>();
	}
}
