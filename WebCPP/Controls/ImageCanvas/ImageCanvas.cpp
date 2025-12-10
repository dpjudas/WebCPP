
#include "WebCPP/Controls/ImageCanvas/ImageCanvas.h"
#include <cmath>

namespace web
{
	ImageCanvas::ImageCanvas() : View("imagecanvas-view")
	{
		canvas = std::make_unique<Element>("canvas");
		element->appendChild(canvas.get());

		context = canvas->handle.call<JSValue>("getContext", std::string("2d"));

		resizeObserver.onResize = [=](std::vector<web::ResizeObserverEntry> entries) { onResize(std::move(entries)); };
		resizeObserver.observe(canvas.get());
	}

	void ImageCanvas::onResize(std::vector<ResizeObserverEntry> entries)
	{
		update();
	}

	void ImageCanvas::update()
	{
		// Find canvas size in physical pixels
		Rect box = canvas->getBoundingClientRect();
		double uiscale = JSValue::global("window")["devicePixelRatio"].as<double>();
		int pixelWidth = (int)std::round(box.width * uiscale);
		int pixelHeight = (int)std::round(box.height * uiscale);
		if (pixelWidth <= 0 || pixelHeight <= 0)
			return;

		// Resize canvas bitmap resolution to match its physical resolution
		if (pixelWidth != image.width || pixelHeight != image.height)
		{
			image.data = context.call<JSValue>("createImageData", pixelWidth, pixelHeight);
			image.buffer.resize(pixelWidth * pixelHeight);
			image.width = pixelWidth;
			image.height = pixelHeight;
			canvas->setAttribute("width", std::to_string(pixelWidth));
			canvas->setAttribute("height", std::to_string(pixelHeight));
		}

		// Render the contents
		paint(image.width, image.height, image.buffer.data(), uiscale);

		// Transfer the result to the ImageData buffer
		image.data["data"].call<void>("set", JSValue(emscripten::typed_memory_view(image.buffer.size() * sizeof(uint32_t), (uint8_t*)image.buffer.data())));

		// Draw the image data
		context.call<void>("putImageData", image.data, 0, 0);
	}
}
