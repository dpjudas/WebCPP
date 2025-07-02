
#include "WebCPP/Controls/Toolbar/Toolbar.h"

namespace web
{
	Toolbar::Toolbar() : View("toolbar-view")
	{
		auto layout = createHBoxLayout();
		layout->setGap(5.0);
	}

	std::shared_ptr<ToolbarButton> Toolbar::addButton(std::string img, std::string text, std::function<void()> onClick)
	{
		auto button = std::make_shared<ToolbarButton>();
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

	std::shared_ptr<ToolbarSeparator> Toolbar::addSeparator()
	{
		auto sep = std::make_shared<ToolbarSeparator>();
		sep->addClass("toolbar-sep");
		getLayout<HBoxLayout>()->addView(sep);
		return sep;
	}

	std::shared_ptr<ToolbarDropdown> Toolbar::addDropdown(std::string label, std::vector<std::string> items)
	{
		auto dropdown = std::make_shared<ToolbarDropdown>();
		dropdown->addClass("toolbar-dropdown");
		dropdown->label->setText(label);
		for (const std::string& item : items)
			dropdown->combobox->addItem(item);
		getLayout<HBoxLayout>()->addView(dropdown);
		return dropdown;
	}

	std::shared_ptr<ToolbarTextInput> Toolbar::addTextInput(std::string label, std::string text)
	{
		auto input = std::make_shared<ToolbarTextInput>();
		input->addClass("toolbar-input");
		input->label->setText(label);
		input->edit->setText(text);
		getLayout<HBoxLayout>()->addView(input);
		return input;
	}

	std::shared_ptr<ToolbarButton> Toolbar::addRadioButton(std::string img)
	{
		auto button = std::make_shared<ToolbarButton>();
		button->addClass("toolbar-radiobutton");
		button->icon->setSrc(img);
		getLayout<HBoxLayout>()->addView(button);
		return button;
	}

	void Toolbar::addView(std::shared_ptr<View> view)
	{
		view->addClass("toolbar-view");
		getLayout<HBoxLayout>()->addView(view);
	}

	void Toolbar::addSpacer()
	{
		getLayout<HBoxLayout>()->addSpacer();
	}
}
