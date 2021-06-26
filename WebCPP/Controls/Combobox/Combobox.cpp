
#include "Combobox.h"
#include <algorithm>

ComboBox::ComboBox(View* parent) : View(parent, "select")
{
	addClass("combobox");
}

void ComboBox::setChangeHandler(const std::function<void()>& handler)
{
	element->addEventListener("change", [=](Event* e) { e->stopPropagation(); handler(); });
}

void ComboBox::addItem(std::string text)
{
	JSValue option = JSValue::global("document").call<JSValue>("createElement", std::string("option"));
	option.set("innerText", text);
	element->handle.call<void>("appendChild", option);
	items.push_back(text);
}

void ComboBox::sort()
{
	while (true)
	{
		JSValue child = element->handle["firstChild"];
		if (child.isNull())
			break;
		element->handle.call<void>("removeChild", child);
	}

	struct SortItem
	{
		SortItem(const std::string& text) : text(text), sortCompareString(text) { }

		std::string text;
		JSValue sortCompareString;
	};

	std::vector<SortItem> sortitems;
	for (const std::string& text : items)
		sortitems.push_back(text);

	std::stable_sort(items.begin(), items.end(), [](const SortItem& a, const SortItem& b) -> bool { return a.sortCompareString.call<int>("localeCompare", b.sortCompareString) < 0; });

	items.clear();
	for (const SortItem& item : sortitems)
	{
		const std::string& text = item.text;
		JSValue option = JSValue::global("document").call<JSValue>("createElement", std::string("option"));
		option.set("innerText", text);
		element->handle.call<void>("appendChild", option);
		items.push_back(text);
	}
}

void ComboBox::setValue(std::string text)
{
	int index = 0;
	for (auto& item : items)
	{
		if (item == text)
		{
			element->handle.set("selectedIndex", index);
			break;
		}
		index++;
	}
}

std::string ComboBox::getValue()
{
	int index = getSelectedIndex();
	if (index != -1)
		return items[index];
	else
		return {};
}

int ComboBox::getSelectedIndex()
{
	return element->handle["selectedIndex"].as<int>();
}
