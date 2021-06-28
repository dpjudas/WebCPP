
#include <WebCPP/Core/DocumentBodyView.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Dialog/WindowHeader.h>
#include <WebCPP/MainWindow/Menubar.h>
#include <WebCPP/MainWindow/Menu.h>

class ApplicationView : public DocumentBodyView
{
public:
	ApplicationView()
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
	}

	void onFileMenu(Menu* menu)
	{
		menu->addItem("", "Open");
		menu->addSeparator();
		menu->addItem("", "Exit");
	}

	void onEditMenu(Menu* menu)
	{
		menu->addItem("", "Cut");
		menu->addItem("", "Copy");
		menu->addItem("", "Paste");
		menu->addItem("", "Delete");
		menu->addSeparator();
		menu->addItem("", "Select All");
	}

	void onViewMenu(Menu* menu)
	{
		menu->addItem("", "Music");
	}

	void onPlaybackMenu(Menu* menu)
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

	void onLibraryMenu(Menu* menu)
	{
		menu->addItem("", "Album List");
		menu->addItem("", "Search");
		menu->addSeparator();
		menu->addItem("", "Configure");
	}

	void onHelpMenu(Menu* menu)
	{
		menu->addItem("", "View Help");
		menu->addSeparator();
		menu->addItem("", "About Music Player");
	}

	WindowHeader* header = nullptr;
	Menubar* menubar = nullptr;
};

int main()
{
	DocumentBodyView::create<ApplicationView>();
	return 0;
}
