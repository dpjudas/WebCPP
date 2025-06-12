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

#pragma once

#include "View.h"

class ModalLayer;

class DocumentBodyView : public View
{
public:
	DocumentBodyView();

	void addView(View* view, bool grow = false, bool shrink = false);

	static DocumentBodyView* get();
	template<typename T>
	static T* get() { return static_cast<T*>(get()); }

	template<typename T>
	static void create() { new T(); }

	ModalLayer* beginModal();
	ModalLayer* beginUnshadedModal();
	void endModal();

	virtual void onNavigate() { }


private:
	std::vector<ModalLayer*> modalLayers;

	static DocumentBodyView* instance;
};

class ModalLayer : public View
{
public:
	ModalLayer() : View("modallayer-view")
	{
	}

	JSValue oldActiveElement = JSValue::undefined();
};

class ShadedModalLayer : public ModalLayer
{
public:
	ShadedModalLayer()
	{
		addClass("shaded");
	}
};
