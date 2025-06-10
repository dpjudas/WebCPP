#pragma once

#include <string>
#include <functional>

class Clipboard
{
public:
	static void setText(const std::string& text);
	static void getText(std::function<void(std::string text)> onResult);
};
