#pragma once

#include "../../Controls/IconBox/IconBox.h"

namespace web
{
	class ImageBox : public IconBox
	{
	public:
		ImageBox();

		void setSrc(std::string src) override;
		void setSize(const int width, const int height);
		void setAlt(const std::string& alternateText);

		std::function<void()> clicked = nullptr;

		static void setResourceUrlBase(const std::string& urlBase);

	private:
		void onClicked(Event* event);

		std::shared_ptr<View> img;

		static std::string resourceUrlBase;
	};
}
