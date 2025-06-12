#pragma once

#include "../../Core/View.h"

enum class ScrollbarDirection
{
	vertical,
	horizontal
};

class Scrollbar : public View
{
public:
	Scrollbar(ScrollbarDirection direction = ScrollbarDirection::vertical);

	ScrollbarDirection getDirection() const { return direction; }
	void setDirection(ScrollbarDirection direction);

	double getPosition() const { return thumbStart; }
	void setPosition(double start, double end);

	std::function<void()> scroll;

private:
	void setupUi();
	void updateThumb();
	void moveTo(double start);
	void onButtonDecrClick(Event* e);
	void onButtonIncrClick(Event* e);
	void onTrackPointerDown(Event* e);
	void onTrackPointerUp(Event* e);
	void onTrackPointerCancel(Event* e);
	void onTrackPointerMove(Event* e);

	ScrollbarDirection direction = ScrollbarDirection::vertical;

	View* buttonDecr = nullptr;
	View* track = nullptr;
	View* thumb = nullptr;
	View* buttonIncr = nullptr;

	double thumbStart = 0.0;
	double thumbEnd = 1.0;

	bool movingThumb = false;
	double moveStart = 0.0;
	double moveThumbStart = 0.0;
};

class ScrollbarCorner : public View
{
public:
	ScrollbarCorner();
};
