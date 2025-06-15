
#include "WebCPP/Controls/Combobox/Combobox.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Core/DocumentBodyView.h"
#include <algorithm>

namespace web
{
	ComboBox::ComboBox() : View("combobox-view")
	{
		imagebox = new ImageBox();
		imagebox->addClass("combobox-icon");
		imagebox->hide();
		label = new TextLabel();
		label->addClass("combobox-label");

		auto layout = createHBoxLayout();
		layout->addView(imagebox);
		layout->addView(label, true, true);

		element->setTabIndex(0);
		element->addEventListener("click", [=](Event* event) { if (getEnabled() == true) onClick(event); });
		element->addEventListener("focus", [=](Event* event) { if (lineedit != nullptr && lineedit->getVisible() == true) { lineedit->setFocus(); event->stopPropagation(); event->preventDefault(); } });
		element->addEventListener("focusin", [=](Event* event) { addClass("focus"); });
	}

	void ComboBox::setEditable()
	{
		if (lineedit == nullptr)
		{
			imagebox->hide();
			label->hide();

			lineedit = new LineEdit();
			lineedit->addClass("combobox-lineedit");
			lineedit->setFlat();
			lineedit->setText(label->getText());
			lineedit->setChangeHandler([=](const std::string& text) { setSelectedIndex(-1); lineedit->setText(text); label->setText(text); if (changeHandler) changeHandler(); });

			getLayout<HBoxLayout>()->addView(lineedit, true, true);

			element->removeTabIndex();
		}
	}

	void ComboBox::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ComboBox::setMaxItems(const int value)
	{
		maxItems = value;
		if (popup != nullptr)
			popup->setMaxItems(maxItems);
	}

	void ComboBox::clearItems()
	{
		items.clear();
		selectedIndex = -1;
	}

	int ComboBox::addItem(std::string text, const std::string& id)
	{
		return addItem({}, text, id);
	}

	int ComboBox::addItem(const std::string& icon, const std::string& text, const std::string& id)
	{
		items.push_back({ icon, text, id });
		if (items.size() == 1)
			setSelectedIndex(0);
		return (items.size() - 1);
	}

	void ComboBox::sort()
	{
		struct SortItem
		{
			SortItem(const std::tuple<std::string, std::string, std::string>& tuple) : tuple(tuple), sortCompareString(std::get<1>(tuple)) {}

			std::tuple<std::string, std::string, std::string> tuple;
			JSValue sortCompareString;
		};

		std::vector<SortItem> sortitems;
		for (const auto& item : items)
			sortitems.push_back(item);

		std::stable_sort(sortitems.begin(), sortitems.end(), [](const SortItem& a, const SortItem& b) -> bool { return a.sortCompareString.call<int>("localeCompare", b.sortCompareString) < 0; });

		items.clear();
		for (const SortItem& item : sortitems)
			addItem(std::get<0>(item.tuple), std::get<1>(item.tuple), std::get<2>(item.tuple));

		setSelectedIndex(0);
	}

	void ComboBox::setValue(const std::string& text)
	{
		int index = 0;
		for (auto& item : items)
		{
			if (std::get<1>(item) == text)
			{
				setSelectedIndex(index);
				break;
			}
			index++;
		}
	}

	void ComboBox::setSelectedIndex(const int index)
	{
		if (index == -1)
		{
			imagebox->hide();
			label->removeClass("combobox-label-hasicon");
			label->setText("");
			if (lineedit != nullptr)
				lineedit->setText("");
			selectedIndex = index;
		}
		else if (index < items.size())
		{
			const auto& item = items.at(index);
			if (std::get<0>(item).empty() == false)
			{
				imagebox->setSrc(std::get<0>(item));
				imagebox->show();
				label->addClass("combobox-label-hasicon");
			}
			else
			{
				imagebox->hide();
				label->removeClass("combobox-label-hasicon");
			}
			label->setText(std::get<1>(item));
			if (lineedit != nullptr)
				lineedit->setText(label->getText());
			selectedIndex = index;
		}
	}

	std::string ComboBox::getId() const
	{
		const int index = getSelectedIndex();
		if (index != -1)
			return std::get<2>(items[index]);
		else
			return label->getText();
	}

	std::string ComboBox::getValue() const
	{
		const int index = getSelectedIndex();
		if (index != -1)
			return std::get<1>(items[index]);
		else
			return label->getText();
	}

	int ComboBox::getSelectedIndex() const
	{
		return selectedIndex;
	}

	bool ComboBox::setFocus()
	{
		element->focus();
		return true;
	}

	void ComboBox::setEnabled(const bool value)
	{
		if (enabled != value)
		{
			if (value)
				removeClass("disabled");
			else
				addClass("disabled");
			enabled = value;
		}
	}

	bool ComboBox::getEnabled() const
	{
		return enabled;
	}

	void ComboBox::onClick(Event* event)
	{
		if (popup == nullptr)
		{
			if (items.empty() == false && (lineedit == nullptr || event->target() == element->handle))
			{
				popup = new ComboBoxPopup(this);
				popup->setMaxItems(getMaxItems());
				for (int idx = 0, size = items.size(); idx < size; idx++)
				{
					const auto& item = items.at(idx);

					popup->addItem(std::get<0>(item), std::get<1>(item), (getSelectedIndex() == idx), [=]() {
						setSelectedIndex(idx);
						closePopup();
						if (changeHandler != nullptr)
							changeHandler();
						});
				}
				auto layer = popup->showUnshadedModal(true);
				layer->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); closePopup(); });
			}
		}
		else
		{
			closePopup();
		}
	}

	void ComboBox::closePopup()
	{
		if (popup != nullptr)
		{
			popup->closeModal();
			popup = nullptr;
			if (lineedit != nullptr)
				lineedit->setFocusWithoutSelect();
			else
				setFocus();
		}
	}

	ComboBoxPopup::ComboBoxPopup(ComboBox* combobox) : View("comboboxpopup-view"), combobox(combobox)
	{
		createVBoxLayout();
		element->setTabIndex(0);
		setDefaultFocus();

		element->addEventListener("keydown", [=](Event* e) { onKeyDown(e); });

		// initial size and position
		const Rect& rect = combobox->element->getBoundingClientRect();
		element->setStyle("left", std::to_string(rect.x) + "px");
		element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
		element->setStyle("width", std::to_string(rect.width - 2) + "px");

		observer.onResize = [=](std::vector<ResizeObserverEntry>) {
			const Rect& rect = combobox->element->getBoundingClientRect();
			element->setStyle("left", std::to_string(rect.x) + "px");
			element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
			};
		observer.observe(combobox->element.get());
	}

	void ComboBoxPopup::scrollToItem(ComboBoxPopupItem* item, ComboboxScrollToHint hint)
	{
		if (item != nullptr)
		{
			double pageHeight = element->clientHeight();
			double offsetTop = item->element->offsetTop();
			double offsetHeight = item->element->offsetHeight();
			if (hint == ComboboxScrollToHint::ensureVisible)
			{
				double scrollTop = element->scrollTop();
				if (offsetTop < scrollTop)
					element->setScrollTop(std::max(offsetTop, 0.0));
				else if (offsetTop + offsetHeight > scrollTop + pageHeight)
					element->setScrollTop(std::max(offsetTop + offsetHeight - pageHeight, 0.0));
			}
			else if (hint == ComboboxScrollToHint::positionAtTop)
			{
				element->setScrollTop(offsetTop);
			}
			else if (hint == ComboboxScrollToHint::positionAtBottom)
			{
				element->setScrollTop(std::max(offsetTop + offsetHeight - pageHeight, 0.0));
			}
			else if (hint == ComboboxScrollToHint::positionAtCenter)
			{
				element->setScrollTop(std::max(offsetTop + (offsetHeight - pageHeight) / 2.0, 0.0));
			}
		}
	}

	void ComboBoxPopup::setMaxItems(const int maxItems)
	{
		element->setStyle("max-height", std::to_string(maxItems * 20) + "px");
	}

	int ComboBoxPopup::getSelectedIndex() const
	{
		for (int idx = 0, size = items.size(); idx < size; idx++)
		{
			if (items.at(idx)->isSelected() == true)
				return idx;
		}
		return -1;
	}

	void ComboBoxPopup::setSelectedIndex(const int index)
	{
		for (int idx = 0, size = items.size(); idx < size; idx++)
			items.at(idx)->setSelected(idx == index);
	}

	ComboBoxPopupItem* ComboBoxPopup::getSelectedItem() const
	{
		for (ComboBoxPopupItem* item : items)
		{
			if (item->isSelected() == true)
				return item;
		}
		return nullptr;
	}

	void ComboBoxPopup::setSelectedItem(const ComboBoxPopupItem* value)
	{
		for (ComboBoxPopupItem* item : items)
			item->setSelected(item == value);
	}

	ComboBoxPopupItem* ComboBoxPopup::addItem(std::string icon, std::string text, const bool selected, std::function<void()> onClick)
	{
		auto item = new ComboBoxPopupItem();
		item->addClass("comboboxpopupitem");
		item->setSelected(selected);
		if (icon.empty() == false)
			item->icon->setSrc(icon);
		else
			item->icon->hide();
		item->text->setText(text);
		item->element->addEventListener("pointerdown", [=](Event* event) { event->preventDefault(); });
		item->element->addEventListener("click", [=](Event* event) {
			event->stopPropagation();
			if (onClick)
				onClick();
			});
		items.push_back(item);
		getLayout<VBoxLayout>()->addView(item);
		return item;
	}

	ComboBoxPopupItemSeparator* ComboBoxPopup::addSeparator()
	{
		auto sep = new ComboBoxPopupItemSeparator();
		sep->addClass("menu-sep");
		getLayout<VBoxLayout>()->addView(sep);
		return sep;
	}

	void ComboBoxPopup::onKeyDown(Event* event)
	{
		bool processed = true;

		const int keyCode = event->getKeyCode();
		if (keyCode == 27) // Escape
		{
			combobox->closePopup();
		}
		else if (keyCode == 13) // Enter
		{
			combobox->setSelectedIndex(getSelectedIndex());
			combobox->closePopup();
		}
		else if (keyCode == 33) // Page up
		{
			setSelectedIndex(std::max(0, getSelectedIndex() - combobox->getMaxItems()));
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtTop);
		}
		else if (keyCode == 34) // Page down
		{
			setSelectedIndex(std::min(count() - 1, getSelectedIndex() + combobox->getMaxItems()));
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtBottom);
		}
		else if (keyCode == 35) // End
		{
			setSelectedIndex(count() - 1);
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtBottom);
		}
		else if (keyCode == 36) // Home
		{
			setSelectedIndex(0);
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtTop);
		}
		else if (keyCode == 38) // Arrow up
		{
			setSelectedIndex(std::max(0, getSelectedIndex() - 1));
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtTop);
		}
		else if (keyCode == 40) // Arrow down
		{
			setSelectedIndex(std::min(count() - 1, getSelectedIndex() + 1));
			scrollToItem(getSelectedItem(), ComboboxScrollToHint::positionAtBottom);
		}
		else
		{
			processed = false;
		}

		if (processed)
		{
			event->preventDefault();
			event->stopPropagation();
		}
	}

	ComboBoxPopupItem::ComboBoxPopupItem() : View("comboboxpopupitem-view")
	{
		icon = new ImageBox();
		text = new TextLabel();

		icon->addClass("comboboxpopupitem-icon");
		text->addClass("comboboxpopupitem-text");

		element->addEventListener("mouseenter", [=](Event* event) { onMouseEnter(event); });

		auto layout = createHBoxLayout();
		layout->addView(icon);
		layout->addView(text);
	}

	void ComboBoxPopupItem::setSelected(const bool value)
	{
		selected = value;
		if (selected == true)
			addClass("comboboxpopup-itemselected");
		else
			removeClass("comboboxpopup-itemselected");
	}

	void ComboBoxPopupItem::onMouseEnter(Event* event)
	{
		static_cast<ComboBoxPopup*>(parent())->setSelectedItem(this);
	}
}
