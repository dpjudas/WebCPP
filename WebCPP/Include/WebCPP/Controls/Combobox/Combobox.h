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

		std::shared_ptr<LineEdit> lineEdit() const { return lineedit; }

	private:
		void onClick(Event* event);

		std::function<void()> changeHandler;
		bool enabled = true;
		int selectedIndex = -1;
		int maxItems = 7;
		std::shared_ptr<ComboBoxPopup> popup;
		std::shared_ptr<ImageBox> imagebox;
		std::shared_ptr<TextLabel> label;
		std::shared_ptr<LineEdit> lineedit;
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
		void setSelectedItem(ComboBoxPopupItem* item);

		int count() const { return items.size(); }

		ComboBoxPopupItem* addItem(std::string icon, std::string text, const bool selected, std::function<void()> onClick);
		ComboBoxPopupItemSeparator* addSeparator();

	private:
		void onKeyDown(Event* event);

	private:
		ComboBox* combobox = nullptr;
		std::vector<std::shared_ptr<ComboBoxPopupItem>> items;
		ResizeObserver observer;
	};

	class ComboBoxPopupItem : public View
	{
	public:
		ComboBoxPopupItem();

		bool isSelected() const { return selected; }
		void setSelected(const bool value);

		std::shared_ptr<ImageBox> icon;
		std::shared_ptr<TextLabel> text;

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
}
