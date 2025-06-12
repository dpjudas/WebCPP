#pragma once

#include "../../Core/View.h"

class Checkbox : public View
{
public:
	Checkbox();

	void setChangeHandler(const std::function<void(bool checked)>& handler);

	void setChecked(bool value);
	bool isChecked() const;
	void setEnabled(bool value);
	bool getEnabled() const;

};
