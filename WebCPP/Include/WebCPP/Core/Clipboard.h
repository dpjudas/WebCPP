#pragma once

#include <string>
#include <functional>

namespace web
{
	class Clipboard
	{
	public:
		static void setText(const std::string& text);
		static void getText(std::function<void(std::string text)> onResult);
	};
}
