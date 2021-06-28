
#include "PlayerPanel.h"
#include "PlayerButton.h"
#include "PlayerProgress.h"
#include "PlayerVolume.h"
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Core/Timer.h>
#include <WebCPP/Core/Native.h>

PlayerPanel::PlayerPanel(View* parent) : HBoxView(parent)
{
	playButton = new PlayerButton(this, "/Resources/Icons/play.svg", [=]() { onPlayClicked(); });
	stopButton = new PlayerButton(this, "/Resources/Icons/stop.svg", [=]() { onStopClicked(); });
	pauseButton = new PlayerButton(this, "/Resources/Icons/pause.svg", [=]() { onPauseClicked(); });
	prevButton = new PlayerButton(this, "/Resources/Icons/skipprev.svg", [=]() { onPrevClicked(); });
	nextButton = new PlayerButton(this, "/Resources/Icons/skipnext.svg", [=]() { onNextClicked(); });

	statusSection = new VBoxView(this);
	statusSection->setExpanding();
	trackTitle = new TextLabel(statusSection);
	progress = new PlayerProgress(statusSection);

	volume = new PlayerVolume(this);

	addClass("player");
	statusSection->addClass("player-statussection");
	trackTitle->addClass("player-tracktitle");
	progress->addClass("player-progress");
	volume->addClass("player-volume");
	playButton->addClass("player-playbutton");
	stopButton->addClass("player-stopbutton");
	pauseButton->addClass("player-pausebutton");
	prevButton->addClass("player-prevbutton");
	nextButton->addClass("player-nextbutton");

	update();
}

void PlayerPanel::trackProgressTick()
{
	progress->timeLabel->setText(toTimeString(secondsPosition) + " / " + toTimeString(secondsLength));
	progress->setProgress(std::min(secondsPosition / (double)secondsLength, 1.0));
	secondsPosition++;

	if (!stopTrackProgress)
	{
		setTimeout([=]() { trackProgressTick(); }, 1000);
	}
	else
	{
		trackingProgress = false;
	}
}

void PlayerPanel::beginTrackProgress()
{
	stopTrackProgress = false;
	if (!trackingProgress)
	{
		trackingProgress = true;
		trackProgressTick();
	}
}

void PlayerPanel::update()
{
	JSValue request = JSValue::object();
	request.set("command", std::string("currentplayback"));

	auto onResponse = [=](JSValue response)
	{
		std::string name = response["name"].as<std::string>();
		trackTitle->setText(name);
		secondsPosition = response["secondsPosition"].as<int>();
		secondsLength = response["secondsLength"].as<int>();
		if (!name.empty())
			beginTrackProgress();
	};

	Native::get()->query(request, onResponse);
}

void PlayerPanel::onStopClicked()
{
}

void PlayerPanel::onPlayClicked()
{
}

void PlayerPanel::onPauseClicked()
{
}

void PlayerPanel::onNextClicked()
{
}

void PlayerPanel::onPrevClicked()
{
}

void PlayerPanel::onProgressClicked(double position)
{
}

void PlayerPanel::onVolumeClicked(double volume)
{
}

std::string PlayerPanel::toTimeString(int totalSeconds)
{
	int seconds = totalSeconds;

	int days = seconds / (60 * 60 * 24);
	seconds -= days * (60 * 60 * 24);

	int hours = seconds / (60 * 60);
	seconds -= hours * (60 * 60);

	int minutes = seconds / 60;
	seconds -= minutes * 60;

	std::string str;

	if (totalSeconds >= 60 * 60 * 24)
	{
		str += std::to_string(days);
		str += ":";
	}

	if (totalSeconds >= 60 * 60)
	{
		if (hours < 10) str += "0";
		str += std::to_string(hours);
		str += ":";
	}

	if (minutes < 10) str += "0";
	str += std::to_string(minutes);
	str += ":";

	if (seconds < 10) str += "0";
	str += std::to_string(seconds);
	return str;
}
