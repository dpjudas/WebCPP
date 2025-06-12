#pragma once

#include "../../Core/ResizeObserver.h"
#include "../../Core/View.h"

class ComboBoxPopup;
class ComboBoxPopupItem;
class ComboBoxPopupItemSeparator;
class ImageBox;
class LineEdit;
class TextLabel;

enum class ComboboxScrollToHint
{
	ensureVisible,
	positionAtTop,
	positionAtBottom,
	positionAtCenter
};

typedef std::tuple<std::string, std::string, std::string> ComboboxItemType; // icon, text, id

class ComboBox : public View
{
public:
	ComboBox();

	void setEditable();

	void setChangeHandler(const std::function<void()>& handler);

	int getMaxItems() const { return maxItems; }
	void setMaxItems(const int value);

	void clearItems();
	int addItem(std::string text, const std::string& id = {});
	int addItem(const std::string& icon, const std::string& text, const std::string& id = {});
	std::vector<ComboboxItemType> getItems() const { return items; }
	void sort();
	void setValue(const std::string& text);
	void setSelectedIndex(const int index);
	std::string getId() const;
	std::string getValue() const;
	int getSelectedIndex() const;

	bool setFocus() override;

	void setEnabled(const bool value);
	bool getEnabled() const;

	void closePopup();

	LineEdit* lineEdit() const { return lineedit; }

private:
	void onClick(Event* event);

	std::function<void()> changeHandler;
	bool enabled = true;
	int selectedIndex = -1;
	int maxItems = 7;
	ComboBoxPopup* popup = nullptr;
	ImageBox* imagebox = nullptr;
	TextLabel* label = nullptr;
	LineEdit* lineedit = nullptr;
	std::vector<ComboboxItemType> items;
};

class ComboBoxPopup : public View
{
public:
	ComboBoxPopup(ComboBox* combobox);

	void scrollToItem(ComboBoxPopupItem* item, ComboboxScrollToHint hint);

	void setMaxItems(const int maxItems);

	int getSelectedIndex() const;
	void setSelectedIndex(const int index);
	ComboBoxPopupItem* getSelectedItem() const;
	void setSelectedItem(const ComboBoxPopupItem* item);

	int count() const { return items.size(); }

	ComboBoxPopupItem* addItem(std::string icon, std::string text, const bool selected, std::function<void()> onClick);
	ComboBoxPopupItemSeparator* addSeparator();

private:
	void onKeyDown(Event* event);

private:
	ComboBox* combobox = nullptr;
	std::vector<ComboBoxPopupItem*> items;
	ResizeObserver observer;
};

class ComboBoxPopupItem : public View
{
public:
	ComboBoxPopupItem();

	bool isSelected() const { return selected; }
	void setSelected(const bool value);

	ImageBox* icon = nullptr;
	TextLabel* text = nullptr;

private:
	void onMouseEnter(Event* event);

	bool selected = false;
};

class ComboBoxPopupItemSeparator : public View
{
public:
	ComboBoxPopupItemSeparator() : View("comboboxpopupitemseparator-view")
	{
	}
};
