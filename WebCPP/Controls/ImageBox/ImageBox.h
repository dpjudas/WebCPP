#pragma once

#include "WebCPP/Core/View.h"

class ImageBox : public View
{
public:
	ImageBox();

	void setEnabled(const bool value);
	bool getEnabled() const;

	void setSrc(std::string src);
	void setSize(const int width, const int height);
	void setAlt(const std::string& alternateText);

	std::function<void()> clicked = nullptr;

	static void setResourceUrlBase(const std::string& urlBase);

private:
	void onClicked(Event* event);

	bool enabled = true;
	static std::string resourceUrlBase;
};
