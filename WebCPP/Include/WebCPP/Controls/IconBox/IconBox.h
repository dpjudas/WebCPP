#pragma once

#include "../../Core/View.h"

#include <functional>
#include <memory>
#include <string>

namespace web
{
	class IconBox : public View
	{
	public:
		IconBox() : View("imagebox-view") {}

		virtual void setSrc(std::string src) = 0;
		virtual bool isExtended() const { return false; }

		using Matcher = std::function<bool(const std::string& src)>;
		using Factory = std::function<std::shared_ptr<IconBox>()>;

		static void setExtendedFactory(Matcher matches, Factory create);
		static bool isExtendedSrc(const std::string& src);
		static std::shared_ptr<IconBox> createExtended();

	private:
		static Matcher extendedMatcher;
		static Factory extendedFactory;
	};

	std::shared_ptr<IconBox> createIconBox(const std::string& src);
}
