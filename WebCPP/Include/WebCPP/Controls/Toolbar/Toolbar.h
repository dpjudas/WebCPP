#pragma once

#include "../../Core/View.h"
#include "../../Controls/TextLabel/TextLabel.h"
#include "../../Controls/Combobox/Combobox.h"
#include "../../Controls/LineEdit/LineEdit.h"
#include "../../Controls/ImageBox/ImageBox.h"

namespace web
{
	class ToolbarButton;
	class ToolbarSeparator;
	class ToolbarDropdown;
	class ToolbarTextInput;
	class TextLabel;
	class ComboBox;
	class LineEdit;

	class Toolbar : public View
	{
	public:
		Toolbar();

		std::shared_ptr<ToolbarButton> addButton(std::string img, std::string text, std::function<void()> onClick);
		std::shared_ptr<ToolbarSeparator> addSeparator();
		std::shared_ptr<ToolbarDropdown> addDropdown(std::string label, std::vector<std::string> items);
		std::shared_ptr<ToolbarTextInput> addTextInput(std::string label, std::string text = std::string());
		std::shared_ptr<ToolbarButton> addRadioButton(std::string img);
		void addView(std::shared_ptr<View> view);
		void addSpacer();
	};

	class ToolbarButton : public View
	{
	public:
		ToolbarButton() : View("toolbarbutton-view")
		{
			icon = std::make_shared<ImageBox>();
			text = std::make_shared<TextLabel>();

			icon->addClass("toolbarbutton-icon");
			text->addClass("toolbarbutton-text");

			auto layout = createHBoxLayout();
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

	class ToolbarSeparator : public View
	{
	public:
		ToolbarSeparator() : View("toolbarseparator-view")
		{
		}
	};

	class ToolbarDropdown : public View
	{
	public:
		ToolbarDropdown() : View("toolbardropdown-view")
		{
			label = std::make_shared<TextLabel>();
			combobox = std::make_shared<ComboBox>();

			label->addClass("toolbardropdown-label");
			combobox->addClass("toolbardropdown-combobox");

			auto layout = createHBoxLayout();
			layout->addView(label);
			layout->addView(combobox);
		}

		std::shared_ptr<TextLabel> label;
		std::shared_ptr<ComboBox> combobox;
	};

	class ToolbarTextInput : public View
	{
	public:
		ToolbarTextInput() : View("toolbartextinput-view")
		{
			label = std::make_shared<TextLabel>();
			edit = std::make_shared<LineEdit>();

			label->addClass("toolbartextinput-label");
			edit->addClass("toolbartextinput-edit");

			auto layout = createHBoxLayout();
			layout->addView(label);
			layout->addView(edit);
		}

		void setPlaceholder(std::string text)
		{
			edit->setPlaceholder(text);
		}

		std::shared_ptr<TextLabel> label;
		std::shared_ptr<LineEdit> edit;
	};
}
