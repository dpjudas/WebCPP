#pragma once

#include "../../Core/View.h"

namespace web
{
	class RadioButton : public View
	{
	public:
		RadioButton();

		void setChangeHandler(const std::function<void(bool checked)>& handler);

		void setId(const std::string& value);
		void setName(const std::string& value);
		void setValue(const std::string& value);
		std::string getValue() const;

		void setChecked(bool value = true);
		bool isChecked() const;
		void setEnabled(bool value);
		bool getEnabled() const;

	};
}
