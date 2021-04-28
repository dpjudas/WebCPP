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

class Toolbar : public HBoxView
{
public:
	Toolbar(View* parent);

	ToolbarButton* addButton(std::string img, std::string text, std::function<void()> onClick);
	ToolbarSeparator* addSeparator();
	ToolbarDropdown* addDropdown(std::string label, std::vector<std::string> options);
	ToolbarTextInput* addTextInput(std::string label, std::string text = std::string());
	ToolbarButton* addRadioButton(std::string img);
	void addView(View* view);
};

class ToolbarButton : public HBoxView
{
public:
	ToolbarButton(View* parent) : HBoxView(parent)
	{
		icon = new ImageBox(this);
		text = new TextLabel(this);

		addClass("toolbarbutton");
		icon->addClass("toolbarbutton-icon");
		text->addClass("toolbarbutton-text");
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

	bool getEnabled() const { return enabled; }

	ImageBox* icon = nullptr;
	TextLabel* text = nullptr;

private:
	bool enabled = true;
};

class ToolbarSeparator : public View
{
public:
	ToolbarSeparator(View* parent) : View(parent)
	{
		addClass("toolbarseparator");
	}
};

class ToolbarDropdown : public HBoxView
{
public:
	ToolbarDropdown(View* parent) : HBoxView(parent)
	{
		label = new TextLabel(this);
		combobox = new ComboBox(this);

		addClass("toolbardropdown");
		label->addClass("toolbardropdown-label");
		combobox->addClass("toolbardropdown-combobox");
	}

	TextLabel* label = nullptr;
	ComboBox* combobox = nullptr;
};

class ToolbarTextInput : public HBoxView
{
public:
	ToolbarTextInput(View* parent) : HBoxView(parent)
	{
		label = new TextLabel(this);
		edit = new LineEdit(this);

		addClass("toolbartextinput");
		label->addClass("toolbartextinput-label");
		edit->addClass("toolbartextinput-edit");
	}

	void setPlaceholder(std::string text)
	{
		edit->setPlaceholder(text);
	}

	TextLabel* label = nullptr;
	LineEdit* edit = nullptr;
};
