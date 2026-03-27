
#include "WebCPP/Controls/Combobox/Combobox.h"
#include "WebCPP/Controls/ImageBox/ImageBox.h"
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Core/HtmlDocument.h"
#include <algorithm>

namespace web
{
	ComboBox::ComboBox() : View("combobox-view")
	{
		imagebox = std::make_shared<ImageBox>();
		imagebox->addClass("combobox-icon");
		imagebox->hide();
		label = std::make_shared<TextLabel>();
		label->addClass("combobox-label");

		auto layout = createHBoxLayout();
		layout->addView(imagebox);
		layout->addView(label, true, true);

		element->setTabIndex(0);
		element->addEventListener("click", std::bind_front(&ComboBox::onClick, this));
		element->addEventListener("focus", std::bind_front(&ComboBox::onFocus, this));
		element->addEventListener("focusin", std::bind_front(&ComboBox::onFocusIn, this));
		element->addEventListener("focusout", std::bind_front(&ComboBox::onFocusOut, this));
	}

	void ComboBox::setEditable()
	{
		if (!isEditable())
		{
			imagebox->hide();
			label->hide();

			lineedit = std::make_shared<LineEdit>();
			lineedit->addClass("combobox-lineedit");
			lineedit->setFlat();
			lineedit->setText(label->getText());
			lineedit->setInputHandler(std::bind_front(&ComboBox::onLineEditInput, this));
			lineedit->setChangeHandler(std::bind_front(&ComboBox::onLineEditChange, this));

			getLayout<HBoxLayout>()->addView(lineedit, true, true);

			element->removeTabIndex();
		}
	}

	void ComboBox::onLineEditInput(const std::string& text)
	{
		setSelectedIndex(-1);
		label->setText(lineedit->getText());
	}

	void ComboBox::onLineEditChange(const std::string& text)
	{
		setSelectedIndex(-1);
		lineedit->setText(text);
		label->setText(text);
		if (changeHandler)
			changeHandler();
	}

	void ComboBox::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ComboBox::setMaxItems(int value)
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
			SortItem(const ComboBoxItem& item) : item(item), sortCompareString(item.text) {}

			ComboBoxItem item;
			JSValue sortCompareString;
		};

		std::vector<SortItem> sortitems;
		for (const auto& item : items)
			sortitems.push_back(item);

		std::stable_sort(sortitems.begin(), sortitems.end(), [](const SortItem& a, const SortItem& b) -> bool { return a.sortCompareString.call<int>("localeCompare", b.sortCompareString) < 0; });

		items.clear();
		for (const SortItem& item : sortitems)
			addItem(item.item.icon, item.item.text, item.item.id);

		setSelectedIndex(0);
	}

	void ComboBox::setValue(const std::string& text)
	{
		int index = 0;
		for (auto& item : items)
		{
			if (item.text == text)
			{
				setSelectedIndex(index);
				return;
			}
			index++;
		}

		setSelectedIndex(-1);
		if (isEditable())
			lineedit->setText(text);
		label->setText(text);
	}

	std::string ComboBox::getValue() const
	{
		int index = getSelectedIndex();
		if (index != -1)
			return items[index].text;
		else if (isEditable())
			return lineedit->getText();
		else
			return label->getText();
	}


	void ComboBox::setSelectedIndex(int index)
	{
		if (index == -1)
		{
			imagebox->hide();
			label->removeClass("combobox-label-hasicon");
			label->setText("");
			if (isEditable())
				lineedit->setText("");
			selectedIndex = index;
		}
		else if (index < items.size())
		{
			const auto& item = items.at(index);
			if (item.icon.empty() == false)
			{
				imagebox->setSrc(item.icon);
				imagebox->show();
				label->addClass("combobox-label-hasicon");
			}
			else
			{
				imagebox->hide();
				label->removeClass("combobox-label-hasicon");
			}
			label->setText(item.text);
			if (isEditable())
				lineedit->setText(label->getText());
			selectedIndex = index;
		}
	}

	std::string ComboBox::getId() const
	{
		int index = getSelectedIndex();
		if (index != -1)
			return items[index].id;
		else
			return {};
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

	void ComboBox::setEnabled(bool value)
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

	void ComboBox::onFocus(Event* event)
	{
		if (lineedit != nullptr && lineedit->getVisible())
		{
			lineedit->setFocus(); event->stopPropagation(); event->preventDefault();
		}
	}

	void ComboBox::onFocusIn(Event* event)
	{
		addClass("focus");
	}

	void ComboBox::onFocusOut(Event* event)
	{
		removeClass("focus");
	}

	void ComboBox::onClick(Event* event)
	{
		if (!getEnabled())
			return;

		if (popup == nullptr)
		{
			if (items.empty() == false && (lineedit == nullptr || event->target() == element->handle))
			{
				popup = std::make_shared<ComboBoxPopup>(this);
				popup->setMaxItems(getMaxItems());
				for (int idx = 0, size = items.size(); idx < size; idx++)
				{
					const auto& item = items.at(idx);
					popup->addItem(item.icon, item.text, (getSelectedIndex() == idx), std::bind_front(&ComboBox::onPopupItemClick, this, idx));
				}
				auto layer = popup->showPopupModal(true);
				layer->element->addEventListener("click", std::bind_front(&ComboBox::onPopupModalLayerClick, this));
			}
		}
		else
		{
			closePopup();
		}
	}

	void ComboBox::onPopupModalLayerClick(Event* event)
	{
		event->stopPropagation();
		closePopup();
	}

	void ComboBox::onPopupItemClick(int index)
	{
		setSelectedIndex(index);
		closePopup();
		if (changeHandler != nullptr)
			changeHandler();
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
		setDefaultFocused();

		element->addEventListener("keydown", std::bind_front(&ComboBoxPopup::onKeyDown, this));

		// initial size and position
		const Rect& rect = combobox->element->getBoundingClientRect();
		element->setStyle("left", std::to_string(rect.x) + "px");
		element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
		element->setStyle("width", std::to_string(rect.width - 2) + "px");

		observer.onResize = std::bind_front(&ComboBoxPopup::onResize, this);
		observer.observe(combobox->element.get());
	}

	void ComboBoxPopup::onResize(std::vector<ResizeObserverEntry> entries)
	{
		const Rect& rect = combobox->element->getBoundingClientRect();
		element->setStyle("left", std::to_string(rect.x) + "px");
		element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
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

	void ComboBoxPopup::setMaxItems(int maxItems)
	{
		element->setStyle("max-height", std::to_string(maxItems * 20) + "px");
	}

	int ComboBoxPopup::getSelectedIndex() const
	{
		for (int idx = 0, size = items.size(); idx < size; idx++)
		{
			if (items.at(idx)->isSelected())
				return idx;
		}
		return -1;
	}

	void ComboBoxPopup::setSelectedIndex(int index)
	{
		for (int idx = 0, size = items.size(); idx < size; idx++)
			items.at(idx)->setSelected(idx == index);
	}

	ComboBoxPopupItem* ComboBoxPopup::getSelectedItem() const
	{
		for (const auto& item : items)
		{
			if (item->isSelected())
				return item.get();
		}
		return nullptr;
	}

	void ComboBoxPopup::setSelectedItem(ComboBoxPopupItem* value)
	{
		for (const auto& item : items)
			item->setSelected(item.get() == value);
	}

	ComboBoxPopupItem* ComboBoxPopup::addItem(std::string icon, std::string text, bool selected, std::function<void()> onClick)
	{
		auto item = std::make_shared<ComboBoxPopupItem>();
		item->addClass("comboboxpopupitem");
		item->setSelected(selected);
		if (icon.empty() == false)
			item->icon->setSrc(icon);
		else
			item->icon->hide();
		item->text->setText(text);
		item->element->addEventListener("pointerdown", std::bind_front(&ComboBoxPopup::onItemPointerDown, this));
		item->element->addEventListener("click", std::bind_front(&ComboBoxPopup::onItemClick, this, onClick));
		items.push_back(item);
		getLayout<VBoxLayout>()->addView(item);
		return item.get();
	}

	ComboBoxPopupItemSeparator* ComboBoxPopup::addSeparator()
	{
		auto sep = std::make_shared<ComboBoxPopupItemSeparator>();
		sep->addClass("menu-sep");
		getLayout<VBoxLayout>()->addView(sep);
		return sep.get();
	}

	void ComboBoxPopup::onItemClick(std::function<void()> onClick, Event* event)
	{
		event->stopPropagation();
		if (onClick)
			onClick();
	}

	void ComboBoxPopup::onItemPointerDown(Event* event)
	{
		event->preventDefault();
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
		icon = std::make_shared<ImageBox>();
		text = std::make_shared<TextLabel>();

		icon->addClass("comboboxpopupitem-icon");
		text->addClass("comboboxpopupitem-text");

		element->addEventListener("mouseenter", std::bind_front(&ComboBoxPopupItem::onMouseEnter, this));

		auto layout = createHBoxLayout();
		layout->addView(icon);
		layout->addView(text);
	}

	void ComboBoxPopupItem::setSelected(bool value)
	{
		selected = value;
		if (selected)
			addClass("comboboxpopup-itemselected");
		else
			removeClass("comboboxpopup-itemselected");
	}

	void ComboBoxPopupItem::onMouseEnter(Event* event)
	{
		static_cast<ComboBoxPopup*>(parent())->setSelectedItem(this);
	}
}
