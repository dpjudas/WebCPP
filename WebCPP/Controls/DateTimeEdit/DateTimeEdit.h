#pragma once

#include "WebCPP/Core/View.h"

class DateTimeEdit : public View
{
public:
	DateTimeEdit();

	void setValue(const std::string& isodatetime);
	std::string getValue();

	void setMin(const std::string& isodatetime);
	std::string getMin();

	void setMax(const std::string& isodatetime);
	std::string getMax();

	void setStep(const int seconds);
	int getStep();

	void setEnabled(const bool value);
	bool getEnabled() const;

	bool setFocus() override;

private:
	bool enabled = true;
};
