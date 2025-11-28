#pragma once

#include "../../Core/View.h"
#include "../../Core/Element.h"
#include "../../Core/ResizeObserver.h"

namespace web
{
	class ImageCanvas : public View
	{
	public:
		ImageCanvas();

		void update();

	protected:
		virtual void paint(int width, int height, uint32_t* pixels, double uiscale) = 0;

	private:
		void onResize(std::vector<ResizeObserverEntry> entries);

		struct
		{
			int width = 0;
			int height = 0;
			std::vector<uint32_t> buffer;
			JSValue data;
		} image;

		std::unique_ptr<Element> canvas;
		JSValue context;
		ResizeObserver resizeObserver;
	};
}
