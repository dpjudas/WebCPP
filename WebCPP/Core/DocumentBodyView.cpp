/*
**  WebCPP framework
**  Copyright (c) 2021 Magnus Norddahl
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
*/

#include "DocumentBodyView.h"
#include "Navigation.h"
#include <stdexcept>

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
