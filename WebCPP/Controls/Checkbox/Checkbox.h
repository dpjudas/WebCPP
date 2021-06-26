#pragma once

#include "WebCPP/Core/View.h"

class Checkbox : public View
{
public:
	Checkbox(View* parent);

	void setChangeHandler(const std::function<void(bool checked)>& handler);

	void setChecked(bool value);
	bool isChecked() const;
	void setEnabled(bool value);
	bool getEnabled() const;

private:
	bool enabled = true;
};
