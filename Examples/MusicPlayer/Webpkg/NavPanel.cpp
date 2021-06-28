
#include "NavPanel.h"
#include "MusicPlayer.h"
#include "PlayerPanel.h"
#include "Playlist.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Controls/ListView/ListView.h>
#include <WebCPP/Controls/ListView/ListViewItem.h>
#include <WebCPP/Controls/ListView/ListViewItemView.h>
#include <WebCPP/Core/Native.h>

NavPanel::NavPanel(MusicPlayer* musicplayer, View* parent) : VBoxView(parent), musicplayer(musicplayer)
{
	addClass("navpanel");
	musicplayer = musicplayer;

	title = new TextLabel(this);
	listview = new ListView(this);
	listview->setExpanding();

	title->addClass("navpanel-title");
	listview->addClass("navpanel-listview");

	title->setText("All Music");
	listview->addColumn("Name", 100, true);
	listview->rootItem()->setId("d:\\\\nas\\music");

	listview->activated = [=](ListViewItem* item) { onActivate(item); };

	updateFolder(listview->rootItem());
}

void NavPanel::updateFolder(ListViewItem* item)
{
	std::string basepath = item->id().substr(2);

	JSValue request = JSValue::object();
	request.set("command", std::string("searchfolder"));
	request.set("path", basepath);

	auto onResponse = [=](JSValue response)
	{
		item->removeAllChildren();

		for (std::string& folder : vecFromJSArray<std::string>(response["folders"]))
		{
			TextListViewItem* subitem = (TextListViewItem*)item->add(std::make_unique<TextListViewItem>("d:" + basepath + "/" + folder));
			subitem->setText(0, folder);
		}

		for (std::string& file : vecFromJSArray<std::string>(response["files"]))
		{
			TextListViewItem* subitem = (TextListViewItem*)item->add(std::make_unique<TextListViewItem>("f:" + basepath + "/" + file));
			subitem->setText(0, file);
		}
	};

	Native::get()->query(request, onResponse);
}

void NavPanel::onActivate(ListViewItem* item)
{
	if (item->id().substr(0, 2) == "f:")
	{
		JSValue request = JSValue::object();
		request.set("command", std::string("play"));
		request.set("filename", item->id().substr(2));

		auto onResponse = [=](JSValue response)
		{
			if (musicplayer->player)
				musicplayer->player->update();
			if (musicplayer->playlist)
				musicplayer->playlist->update();
		};

		Native::get()->query(request, onResponse);
	}
	else if (item->id().substr(0, 2) == "d:")
	{
		updateFolder(item);
	}
}
