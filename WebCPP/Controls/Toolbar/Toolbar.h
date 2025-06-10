#pragma once

#include "WebCPP/Core/View.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Controls/Combobox/Combobox.h"
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"

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

	ToolbarButton* addButton(std::string img, std::string text, std::function<void()> onClick);
	ToolbarSeparator* addSeparator();
	ToolbarDropdown* addDropdown(std::string label, std::vector<std::string> items);
	ToolbarTextInput* addTextInput(std::string label, std::string text = std::string());
	ToolbarButton* addRadioButton(std::string img);
	void addView(View* view);
	void addSpacer();
};

class ToolbarButton : public View
{
public:
	ToolbarButton() : View("toolbarbutton-view")
	{
		icon = new ImageBox();
		text = new TextLabel();

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

	ImageBox* icon = nullptr;
	TextLabel* text = nullptr;

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
		label = new TextLabel();
		combobox = new ComboBox();

		label->addClass("toolbardropdown-label");
		combobox->addClass("toolbardropdown-combobox");

		auto layout = createHBoxLayout();
		layout->addView(label);
		layout->addView(combobox);
	}

	TextLabel* label = nullptr;
	ComboBox* combobox = nullptr;
};

class ToolbarTextInput : public View
{
public:
	ToolbarTextInput() : View("toolbartextinput-view")
	{
		label = new TextLabel();
		edit = new LineEdit();

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

	TextLabel* label = nullptr;
	LineEdit* edit = nullptr;
};
