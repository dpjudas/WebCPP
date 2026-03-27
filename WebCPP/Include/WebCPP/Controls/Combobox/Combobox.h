#pragma once

#include "../../Core/ResizeObserver.h"
#include "../../Core/View.h"

namespace web
{
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

	struct ComboBoxItem
	{
		std::string icon;
		std::string text;
		std::string id;
	};

	class ComboBox : public View
	{
	public:
		ComboBox();

		void setEditable();
		bool isEditable() const { return lineedit != nullptr; }

		void setChangeHandler(const std::function<void()>& handler);

		int getMaxItems() const { return maxItems; }
		void setMaxItems(const int value);

		void clearItems();
		int addItem(std::string text, const std::string& id = {});
		int addItem(const std::string& icon, const std::string& text, const std::string& id = {});
		std::vector<ComboBoxItem> getItems() const { return items; }
		void sort();
		void setValue(const std::string& text);
		void setSelectedIndex(int index);
		std::string getId() const;
		std::string getValue() const;
		int getSelectedIndex() const;

		bool setFocus() override;

		void setEnabled(bool value);
		bool getEnabled() const;

		void closePopup();

		std::shared_ptr<LineEdit> lineEdit() const { return lineedit; }

	private:
		void onClick(Event* event);
		void onFocus(Event* event);
		void onFocusIn(Event* event);
		void onFocusOut(Event* event);
		void onLineEditInput(const std::string& text);
		void onLineEditChange(const std::string& text);
		void onPopupModalLayerClick(Event* event);
		void onPopupItemClick(int index);
		void updateSelectedIndexFromEdit();

		std::function<void()> changeHandler;
		bool enabled = true;
		int selectedIndex = -1;
		int maxItems = 7;
		std::shared_ptr<ComboBoxPopup> popup;
		std::shared_ptr<ImageBox> imagebox;
		std::shared_ptr<TextLabel> label;
		std::shared_ptr<LineEdit> lineedit;
		std::vector<ComboBoxItem> items;
	};

	class ComboBoxPopup : public View
	{
	public:
		ComboBoxPopup(ComboBox* combobox);

		void scrollToItem(ComboBoxPopupItem* item, ComboboxScrollToHint hint);

		void setMaxItems(const int maxItems);

		int getSelectedIndex() const;
		void setSelectedIndex(int index);
		ComboBoxPopupItem* getSelectedItem() const;
		void setSelectedItem(ComboBoxPopupItem* item);

		int count() const { return items.size(); }

		ComboBoxPopupItem* addItem(std::string icon, std::string text, bool selected, std::function<void()> onClick);
		ComboBoxPopupItemSeparator* addSeparator();

	private:
		void onItemPointerDown(Event* event);
		void onItemClick(std::function<void()> onClick, Event* event);
		void onKeyDown(Event* event);
		void onResize(std::vector<ResizeObserverEntry> entries);

		ComboBox* combobox = nullptr;
		std::vector<std::shared_ptr<ComboBoxPopupItem>> items;
		ResizeObserver observer;
	};

	class ComboBoxPopupItem : public View
	{
	public:
		ComboBoxPopupItem();

		bool isSelected() const { return selected; }
		void setSelected(bool value);

		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> text;

	private:
		void onMouseEnter(Event* event);

		bool selected = false;
	};

	class ComboBoxPopupItemSeparator : public View
	{
	public:
		ComboBoxPopupItemSeparator() : View("comboboxpopupitemseparator-view") {}
	};
}
