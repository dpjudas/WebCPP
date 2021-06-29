#pragma once

#include <WebCPP/Core/View.h>

class ImageBox;
class TextLabel;
class PlayerProgress;
class PlayerVolume;

class PlayerPanel : public HBoxView
{
public:
	PlayerPanel(View* parent);

	void update();

private:
	void trackProgressTick();
	void beginTrackProgress();
	void onStopClicked();
	void onPlayClicked();
	void onPauseClicked();
	void onNextClicked();
	void onProgressClicked(double position);
	void onVolumeClicked(double position);

	static std::string toTimeString(int totalSeconds);

	int secondsPosition = 0;
	int secondsLength = 0;
	bool trackingProgress = false;
	bool stopTrackProgress = true;

	ImageBox* playButton = nullptr;
	ImageBox* stopButton = nullptr;
	ImageBox* pauseButton = nullptr;
	ImageBox* nextButton = nullptr;
	VBoxView* statusSection = nullptr;
	TextLabel* trackTitle = nullptr;
	PlayerProgress* progress = nullptr;
	PlayerVolume* volume = nullptr;
};
