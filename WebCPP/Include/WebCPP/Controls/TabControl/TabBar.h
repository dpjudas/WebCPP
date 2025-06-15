#pragma once

#include "../../Core/View.h"

namespace web
{
	class TabBarTab;

	class TabBar : public View
	{
	public:
		TabBar();

		TabBarTab* addTab(const std::string& icon, const std::string& label, std::function<void()> onClick, std::function<void(double clientX, double clientY)> onContextMenu);
	};
}
