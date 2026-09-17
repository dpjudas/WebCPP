#pragma once

#include "../../Core/View.h"
#include <functional>

namespace web
{
	// Horizontal split view with a draggable divider.
	class SplitView : public View
	{
	public:
		// fixSecond=false (default): first panel is fixed width, second fills remaining space.
		// fixSecond=true: second panel is fixed width, first fills remaining space.
		SplitView(std::shared_ptr<View> first, std::shared_ptr<View> second, double initialWidth = 200.0, bool fixSecond = false);

		double firstWidth() const;
		void setFirstWidth(double width);

		double secondWidth() const;
		void setSecondWidth(double width);

		// Floors the divider can't be dragged past (0 = no floor, the default for both). Applies to whichever panel is fixed directly, and to the growing panel by capping how far the divider can encroach on it. Re-clamps the current position immediately if it now violates one
		void setMinSize(double minFirstWidth, double minSecondWidth);

		std::function<void(double)> resized;

	private:
		double clampWidth(double width) const;
		void applyWidth();
		void onPointerDown(Event* e);
		void onPointerUp(Event* e);
		void onPointerCancel(Event* e);
		void onPointerMove(Event* e);

		std::shared_ptr<View> firstView;
		std::shared_ptr<View> divider;
		std::shared_ptr<View> secondView;

		bool fixSecond = false;
		double minFirstWidth = 0.0;
		double minSecondWidth = 0.0;
		double currentWidth;
		double dragStartX = 0.0;
		double dragStartWidth = 0.0;
		bool dragging = false;
	};
}
