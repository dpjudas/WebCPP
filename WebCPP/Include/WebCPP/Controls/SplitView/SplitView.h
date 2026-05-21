#pragma once

#include "../../Core/View.h"
#include <functional>

namespace web
{
	// Horizontal split view with a draggable divider.
	// Pass fully-configured views (with their own layouts) to the constructor
	// so WebCPP's LayoutStyles can capture their display:flex styles correctly.
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

		void setDividerWidth(double width);

		std::function<void(double)> resized;

	private:
		void applyWidth();
		void onPointerDown(Event* e);
		void onPointerUp(Event* e);
		void onPointerCancel(Event* e);
		void onPointerMove(Event* e);

		std::shared_ptr<View> firstView;
		std::shared_ptr<View> divider;
		std::shared_ptr<View> secondView;

		bool fixSecond = false;
		double currentWidth;
		double dragStartX = 0.0;
		double dragStartWidth = 0.0;
		bool dragging = false;
	};
}
