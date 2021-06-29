
#include "Playlist.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Controls/ListView/ListView.h>
#include <WebCPP/Controls/ListView/ListViewItem.h>
#include <WebCPP/Controls/ListView/ListViewItemView.h>
#include <WebCPP/Core/Native.h>

Playlist::Playlist(View* parent) : VBoxView(parent)
{
	title = new TextLabel(this);
	listview = new ListView(this);
	listview->setExpanding();

	addClass("playlist");
	title->addClass("playlist-title");
	listview->addClass("playlist-listview");

	title->setText("Play list");
	listview->addColumn("Name", 250, true);
	listview->addColumn("Artist/album", 250);
	listview->addColumn("Track no", 75);
	listview->addColumn("Title / track artist", 250);
	listview->addColumn("Duration", 150);

	listview->activated = [=](ListViewItem* item) { onItemClicked(item); };

	update();
}

void Playlist::update()
{
	JSValue request = JSValue::object();
	request.set("command", std::string("playlist"));

	auto onResponse = [=](JSValue response)
	{
		listview->clearList();

		for (JSValue& itemjson : vecFromJSArray<JSValue>(response["items"]))
		{
			std::string name = itemjson["name"].as<std::string>();
			TextListViewItem* item = (TextListViewItem*)listview->rootItem()->add(std::make_unique<TextListViewItem>(name));
			item->setIcon(0, "/Resources/Icons/song.svg");
			item->setText(0, name);
			item->setText(1, "Unknown artist");
			item->setText(2, "1");
			item->setText(3, "Unknown title");
			item->setText(4, "00:00");
		}
	};

	Native::get()->query(request, onResponse);
}

void Playlist::onItemClicked(ListViewItem* item)
{
	// std::string name = item->id();
}
