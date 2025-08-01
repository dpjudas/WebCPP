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

#include <string>
#include "View.h"

namespace web
{
	class HtmlDocumentBody;
	class ModalLayer;

	class HtmlDocument
	{
	public:
		static void setTitle(const std::string& title);
		static std::string getTitle();

		static void setIcon(const std::string& href);
		static std::string getIcon();

		static HtmlDocumentBody* body();
	};

	class HtmlDocumentBody : public View
	{
	public:
		HtmlDocumentBody();
		void addView(std::shared_ptr<View> view);

	private:
		std::shared_ptr<ModalLayer> beginDialogModal();
		std::shared_ptr<ModalLayer> beginPopupModal();
		void endModal();

		std::vector<std::shared_ptr<ModalLayer>> modalLayers;
		friend class View;
	};

	class ModalLayer : public View
	{
	public:
		ModalLayer(bool dialog);
		void addView(std::shared_ptr<View> view);

	private:
		JSValue oldActiveElement = JSValue::undefined();
		friend class HtmlDocumentBody;
	};
}
