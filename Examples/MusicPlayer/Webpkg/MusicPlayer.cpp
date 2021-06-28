
#include "MusicPlayer.h"
#include "PlayerPanel.h"
#include "NavPanel.h"
#include "Playlist.h"
#include "PodcastEpisode.h"
#include <WebCPP/Dialog/WindowHeader.h>
#include <WebCPP/MainWindow/Menubar.h>
#include <WebCPP/MainWindow/Menu.h>
#include <WebCPP/Controls/BorderLayoutView/BorderLayoutView.h>

MusicPlayer::MusicPlayer()
{
	addClass("application");

	header = new WindowHeader(this);
	header->addClass("application-header");
	header->caption->setText("Music Player");

	menubar = new Menubar(this);
	menubar->addItem("File", [=](Menu* menu) { onFileMenu(menu); });
	menubar->addItem("Edit", [=](Menu* menu) { onEditMenu(menu); });
	menubar->addItem("View", [=](Menu* menu) { onViewMenu(menu); });
	menubar->addItem("Playback", [=](Menu* menu) { onPlaybackMenu(menu); });
	menubar->addItem("Library", [=](Menu* menu) { onLibraryMenu(menu); });
	menubar->addItem("Help", [=](Menu* menu) { onHelpMenu(menu); });

	playlist = new Playlist(nullptr);
	// episode = new PodcastEpisode(nullptr);

	borderlayout = new BorderLayoutView(this);
	borderlayout->setLeftView(new NavPanel(this, nullptr), 450);
	borderlayout->setCenterView(playlist);
	// borderlayout->setCenterView(episode);

	player = new PlayerPanel(this);
}

void MusicPlayer::onFileMenu(Menu* menu)
{
	menu->addItem("", "Open");
	menu->addSeparator();
	menu->addItem("", "Exit");
}

void MusicPlayer::onEditMenu(Menu* menu)
{
	menu->addItem("", "Cut");
	menu->addItem("", "Copy");
	menu->addItem("", "Paste");
	menu->addItem("", "Delete");
	menu->addSeparator();
	menu->addItem("", "Select All");
}

void MusicPlayer::onViewMenu(Menu* menu)
{
	menu->addItem("", "Music");
}

void MusicPlayer::onPlaybackMenu(Menu* menu)
{
	menu->addItem("", "Stop");
	menu->addItem("", "Pause");
	menu->addItem("", "Play");
	menu->addItem("", "Previous");
	menu->addItem("", "Next");
	menu->addItem("", "Random");
	menu->addSeparator();
	menu->addItem("", "Stop After Current");
}

void MusicPlayer::onLibraryMenu(Menu* menu)
{
	menu->addItem("", "Album List");
	menu->addItem("", "Search");
	menu->addSeparator();
	menu->addItem("", "Configure");
}

void MusicPlayer::onHelpMenu(Menu* menu)
{
	menu->addItem("", "View Help");
	menu->addSeparator();
	menu->addItem("", "About Music Player");
}
