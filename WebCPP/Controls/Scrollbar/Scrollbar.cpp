
#include "Scrollbar.h"

Scrollbar::Scrollbar(ScrollbarDirection direction) : View("scrollbar-view"), direction(direction)
{
	setupUi();

	buttonDecr->element->addEventListener("click", [=](Event* e) { onButtonDecrClick(e); });
	buttonIncr->element->addEventListener("click", [=](Event* e) { onButtonIncrClick(e); });
	track->element->addEventListener("pointerdown", [=](Event* e) { onTrackPointerDown(e); });
	track->element->addEventListener("pointerup", [=](Event* e) { onTrackPointerUp(e); });
	track->element->addEventListener("pointercancel", [=](Event* e) { onTrackPointerCancel(e); });
	track->element->addEventListener("pointermove", [=](Event* e) { onTrackPointerMove(e); });
}

void Scrollbar::onButtonDecrClick(Event* e)
{
	e->stopPropagation();
	e->preventDefault();

	double lineLength = (thumbEnd - thumbStart) / 20;
	moveTo(thumbStart - lineLength);
}

void Scrollbar::onButtonIncrClick(Event* e)
{
	e->stopPropagation();
	e->preventDefault();

	double lineLength = (thumbEnd - thumbStart) / 20;
	moveTo(thumbStart + lineLength);
}

void Scrollbar::moveTo(double start)
{
	double thumbLength = thumbEnd - thumbStart;
	start = std::min(start, 1.0 - thumbLength);
	start = std::max(start, 0.0);
	if (start != thumbStart)
	{
		setPosition(start, start + thumbLength);
		if (scroll)
			scroll();
	}
}

void Scrollbar::onTrackPointerDown(Event* e)
{
	e->stopPropagation();
	e->preventDefault();

	track->element->setPointerCapture(e->pointerId());

	double clickPos = (direction == ScrollbarDirection::vertical) ? e->handle["clientY"].as<double>() : e->handle["clientX"].as<double>();

	Rect trackBox = track->element->getBoundingClientRect();
	double trackStart = (direction == ScrollbarDirection::vertical) ? trackBox.y : trackBox.x;
	double trackLength = (direction == ScrollbarDirection::vertical) ? trackBox.height : trackBox.width;

	double thumbClientStart = trackStart + thumbStart * trackLength;
	double thumbClientEnd = trackStart + thumbEnd * trackLength;

	if (clickPos < thumbClientStart)
	{
		double pageLength = thumbEnd - thumbStart;
		moveTo(thumbStart - pageLength);
	}
	else if (clickPos >= thumbClientEnd)
	{
		double pageLength = thumbEnd - thumbStart;
		moveTo(thumbStart + pageLength);
	}
	else
	{
		movingThumb = true;
		moveStart = (direction == ScrollbarDirection::vertical) ? e->handle["clientY"].as<double>() : e->handle["clientX"].as<double>();
		moveThumbStart = thumbStart;
	}
}

void Scrollbar::onTrackPointerUp(Event* e)
{
	movingThumb = false;
	e->stopPropagation();
	e->preventDefault();
}

void Scrollbar::onTrackPointerCancel(Event* e)
{
	movingThumb = false;
	e->stopPropagation();
}

void Scrollbar::onTrackPointerMove(Event* e)
{
	if (movingThumb)
	{
		double movePos = (direction == ScrollbarDirection::vertical) ? e->handle["clientY"].as<double>() : e->handle["clientX"].as<double>();
		double moveDelta = movePos - moveStart;

		Rect trackBox = track->element->getBoundingClientRect();
		double trackLength = (direction == ScrollbarDirection::vertical) ? trackBox.height : trackBox.width;

		moveTo(moveThumbStart + moveDelta / trackLength);
	}
}

void Scrollbar::updateThumb()
{
	double start = thumbStart;
	double end = 1.0 - thumbEnd;
	if (direction == ScrollbarDirection::vertical)
	{
		thumb->element->setStyle("top", std::to_string(start * 100.0) + "%");
		thumb->element->setStyle("bottom", std::to_string(end * 100.0) + "%");
	}
	else
	{
		thumb->element->setStyle("left", std::to_string(start * 100.0) + "%");
		thumb->element->setStyle("right", std::to_string(end * 100.0) + "%");
	}
}

void Scrollbar::setPosition(double start, double end)
{
	start = std::max(std::min(start, 1.0), 0.0);
	end = std::max(std::min(end, 1.0), 0.0);
	end = std::max(start, end);
	if (thumbStart != start || thumbEnd != end)
	{
		thumbStart = start;
		thumbEnd = end;
		updateThumb();
	}
}

void Scrollbar::setDirection(ScrollbarDirection newDirection)
{
	if (direction != newDirection)
	{
		direction = newDirection;
		if (direction == ScrollbarDirection::vertical)
		{
			removeClass("scrollbar-horz");
			addClass("scrollbar-vert");
			getLayout<FlexLayout>()->setDirection(FlexDirection::column);
		}
		else
		{
			removeClass("scrollbar-vert");
			addClass("scrollbar-horz");
			getLayout<FlexLayout>()->setDirection(FlexDirection::row);
		}
	}
}

void Scrollbar::setupUi()
{
	addClass(direction == ScrollbarDirection::vertical ? "scrollbar-vert" : "scrollbar-horz");

	buttonDecr = new View();
	track = new View();
	thumb = new View();
	buttonIncr = new View();

	buttonDecr->addClass("scrollbar-button-decr");
	track->addClass("scrollbar-track");
	thumb->addClass("scrollbar-thumb");
	buttonIncr->addClass("scrollbar-button-incr");

	auto layout = createFlexLayout();
	layout->setDirection(direction == ScrollbarDirection::vertical ? FlexDirection::column : FlexDirection::row);
	layout->addView(buttonDecr);
	layout->addView(track, true, true);
	layout->addView(buttonIncr);

	auto trackLayout = track->createFlowLayout();
	trackLayout->addAbsoluteView(thumb);
}

////////////////////////////////////////////////////////////////////////////

ScrollbarCorner::ScrollbarCorner() : View("scrollbarcorner-view")
{
}
