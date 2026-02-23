#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "DialogHeaderButton.h"

namespace web
{
	class TextLabel;
	class DialogHeaderButton;

	class DialogHeader : public View
	{
	public:
		DialogHeader();

		std::shared_ptr<TextLabel> caption;
		std::shared_ptr<DialogHeaderButton> closeButton;

		static const std::string CloseIconSvg;
		static const std::string MaximizedIconSvg;
		static const std::string MaximizeIconSvg;
		static const std::string MinimizeIconSvg;
	};
}
