
#include "WebCPP/Controls/Toolbar/Toolbar.h"

Toolbar::Toolbar() : View("toolbar-view")
{
	auto layout = createHBoxLayout();
	layout->setGap(5.0);
}

ToolbarButton* Toolbar::addButton(std::string img, std::string text, std::function<void()> onClick)
{
	auto button = new ToolbarButton();
	button->addClass("toolbar-button");
	button->icon->setSrc(img);
	if (img.empty())
		button->icon->setVisible(false);
	button->text->setText(text);
	if (onClick)
	{
		button->element->addEventListener("mousedown", [=](Event* event) { event->preventDefault(); });
		button->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); if (button->getEnabled()) onClick(); });
	}
	getLayout<HBoxLayout>()->addView(button);
	return button;
}

ToolbarSeparator* Toolbar::addSeparator()
{
	auto sep = new ToolbarSeparator();
	sep->addClass("toolbar-sep");
	getLayout<HBoxLayout>()->addView(sep);
	return sep;
}

ToolbarDropdown* Toolbar::addDropdown(std::string label, std::vector<std::string> items)
{
	auto dropdown = new ToolbarDropdown();
	dropdown->addClass("toolbar-dropdown");
	dropdown->label->setText(label);
	for (const std::string& item : items)
		dropdown->combobox->addItem(item);
	getLayout<HBoxLayout>()->addView(dropdown);
	return dropdown;
}

ToolbarTextInput* Toolbar::addTextInput(std::string label, std::string text)
{
	auto input = new ToolbarTextInput();
	input->addClass("toolbar-input");
	input->label->setText(label);
	input->edit->setText(text);
	getLayout<HBoxLayout>()->addView(input);
	return input;
}

ToolbarButton* Toolbar::addRadioButton(std::string img)
{
	auto button = new ToolbarButton();
	button->addClass("toolbar-radiobutton");
	button->icon->setSrc(img);
	getLayout<HBoxLayout>()->addView(button);
	return button;
}

void Toolbar::addView(View* view)
{
	view->setParent(this);
	view->addClass("toolbar-view");
	getLayout<HBoxLayout>()->addView(view);
}

void Toolbar::addSpacer()
{
	getLayout<HBoxLayout>()->addSpacer();
}
