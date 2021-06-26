#pragma once

#include "WebCPP/Core/View.h"

class ComboBox : public View
{
public:
	ComboBox(View* parent);

	void setChangeHandler(const std::function<void()>& handler);

	void addItem(std::string text);
	void sort();
	void setValue(std::string text);
	std::string getValue();
	int getSelectedIndex();

	std::vector<std::string> items;
};
