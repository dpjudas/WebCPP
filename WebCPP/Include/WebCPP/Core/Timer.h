#pragma once

#include <functional>

namespace web
{
	int setTimeout(std::function<void()> onTimeout, int delay);
	void clearTimeout(int timeoutID);

	int setInterval(std::function<void()> onInterval, int delay);
	void clearInterval(int intervalID);
}
