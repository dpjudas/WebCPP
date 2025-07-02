#pragma once

#include "../../Core/View.h"
#include "../../Controls/TextLabel/TextLabel.h"
#include "../../Controls/ImageBox/ImageBox.h"

namespace web
{
	class VToolbarButton;

	class VToolbar : public View
	{
	public:
		VToolbar();

		std::shared_ptr<VToolbarButton> addButton(std::string img, std::string text, std::function<void()> onClick);
	};

	class VToolbarButton : public View
	{
	public:
		VToolbarButton() : View("vtoolbarbutton-view")
		{
			icon = std::make_shared<ImageBox>();
			text = std::make_shared<TextLabel>();

			icon->addClass("vtoolbarbutton-icon");
			text->addClass("vtoolbarbutton-text");

			auto layout = createVBoxLayout();
			layout->addView(icon);
			layout->addView(text);
		}

		void setEnabled(bool value)
		{
			if (enabled != value)
			{
				enabled = value;
				if (enabled)
					removeClass("disabled");
				else
					addClass("disabled");
			}
		}

		void setSelected(bool value)
		{
			if (selected != value)
			{
				selected = value;
				if (selected)
					addClass("selected");
				else
					removeClass("selected");
			}
		}

		bool getEnabled() const { return enabled; }
		bool getSelected() const { return selected; }

		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> text;

	private:
		bool enabled = true;
		bool selected = false;
	};
}
