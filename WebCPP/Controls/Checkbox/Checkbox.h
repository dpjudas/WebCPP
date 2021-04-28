#pragma once

#include "WebCPP/Core/View.h"

class Checkbox : public View
{
public:
	Checkbox(View* parent);

	void setValue(bool value);
	bool getValue();
	void setEnabled(bool value);
	bool getEnabled() const;

private:
	bool enabled = true;
};
