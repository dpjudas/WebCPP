
#include "WebCPP/Core/DocumentBodyView.h"
#include "WebCPP/Core/Navigation.h"
#include <stdexcept>

namespace web
{
	DocumentBodyView::DocumentBodyView() : View(std::make_unique<Element>(JSValue::global("document")["body"]))
	{
		createVBoxLayout();

		instance = this;
		Navigation::init([=]() { onNavigate(); });
	}

	void DocumentBodyView::addView(View* view, bool grow, bool shrink)
	{
		getLayout<VBoxLayout>()->addView(view, grow, shrink);
	}

	ModalLayer* DocumentBodyView::beginModal()
	{
		ModalLayer* layer = new ShadedModalLayer();
		getLayout<VBoxLayout>()->addView(layer);

		layer->oldActiveElement = JSValue::global("document")["activeElement"];
		modalLayers.push_back(layer);
		return layer;
	}

	ModalLayer* DocumentBodyView::beginUnshadedModal()
	{
		ModalLayer* layer = new ModalLayer();
		getLayout<VBoxLayout>()->addView(layer);

		layer->oldActiveElement = JSValue::global("document")["activeElement"];
		modalLayers.push_back(layer);
		return layer;
	}

	void DocumentBodyView::endModal()
	{
		if (!modalLayers.empty())
		{
			ModalLayer* layer = modalLayers.back();
			modalLayers.pop_back();
			JSValue oldActiveElement = std::move(layer->oldActiveElement);
			delete layer;
			if (!oldActiveElement.isNull())
				oldActiveElement.call<void>("focus");
		}
	}

	DocumentBodyView* DocumentBodyView::get()
	{
		return instance;
	}

	DocumentBodyView* DocumentBodyView::instance = nullptr;
}
