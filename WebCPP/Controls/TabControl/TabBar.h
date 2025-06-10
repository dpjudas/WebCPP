#pragma once

#include "WebCPP/Core/View.h"

class TabBarTab;

class TabBar : public View
{
public:
	TabBar();

	TabBarTab* addTab(const std::string& icon, const std::string& label, std::function<void()> onClick, std::function<void(double clientX, double clientY)> onContextMenu);
};
