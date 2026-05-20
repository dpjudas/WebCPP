
#include "WebCPP/Controls/SplitView/SplitView.h"
#include <algorithm>
#include <cmath>

namespace web
{
	SplitView::SplitView(std::shared_ptr<View> first, std::shared_ptr<View> second, double initialWidth, bool fixSecond)
		: View("splitview-view"), firstView(std::move(first)), secondView(std::move(second)), fixSecond(fixSecond), currentWidth(initialWidth)
	{
		divider = std::make_shared<View>("splitview-divider-view");

		applyWidth();

		auto layout = createHBoxLayout();
		if (fixSecond)
		{
			layout->addView(firstView, true, true, FlexAlignSelf::stretch);
			layout->addView(divider, false, false, FlexAlignSelf::stretch);
			layout->addView(secondView, false, false, FlexAlignSelf::stretch);
		}
		else
		{
			layout->addView(firstView, false, false, FlexAlignSelf::stretch);
			layout->addView(divider, false, false, FlexAlignSelf::stretch);
			layout->addView(secondView, true, true, FlexAlignSelf::stretch);
		}

		divider->element->addEventListener("pointerdown", std::bind_front(&SplitView::onPointerDown, this));
		divider->element->addEventListener("pointerup", std::bind_front(&SplitView::onPointerUp, this));
		divider->element->addEventListener("pointercancel", std::bind_front(&SplitView::onPointerCancel, this));
		divider->element->addEventListener("pointermove", std::bind_front(&SplitView::onPointerMove, this));
	}

	double SplitView::firstWidth() const { return currentWidth; }
	double SplitView::secondWidth() const { return currentWidth; }

	void SplitView::setFirstWidth(double width)
	{
		currentWidth = std::max(width, 0.0);
		applyWidth();
	}

	void SplitView::setSecondWidth(double width)
	{
		currentWidth = std::max(width, 0.0);
		applyWidth();
	}

	void SplitView::applyWidth()
	{
		View* target = fixSecond ? secondView.get() : firstView.get();
		const std::string px = std::to_string(static_cast<int>(std::round(currentWidth))) + "px";
		target->element->setStyle("width", px);
		target->element->setStyle("min-width", px);
		target->element->setStyle("max-width", px);
		target->element->setStyle("flex-shrink", "0");
		target->element->setStyle("flex-grow", "0");
	}

	void SplitView::onPointerDown(Event* e)
	{
		e->stopPropagation();
		e->preventDefault();
		dragging = true;
		dragStartX = e->handle["clientX"].as<double>();
		dragStartWidth = currentWidth;
		divider->element->setPointerCapture(e->pointerId());
		divider->addClass("dragging");
	}

	void SplitView::onPointerUp(Event* e)
	{
		if (dragging)
		{
			dragging = false;
			divider->removeClass("dragging");
			if (resized)
				resized(currentWidth);
		}
		e->stopPropagation();
		e->preventDefault();
	}

	void SplitView::onPointerCancel(Event* e)
	{
		dragging = false;
		divider->removeClass("dragging");
		e->stopPropagation();
	}

	void SplitView::onPointerMove(Event* e)
	{
		if (dragging)
		{
			const double pos = e->handle["clientX"].as<double>();
			const double diff = pos - dragStartX;
			if (fixSecond)
				setSecondWidth(dragStartWidth - diff);
			else
				setFirstWidth(dragStartWidth + diff);
		}
	}
}
