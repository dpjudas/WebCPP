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

#include "HtmlDocument.h"
#include "JSValue.h"

void HtmlDocument::setTitle(const std::string& title)
{
	JSValue::global("document").set("title", title);
}

std::string HtmlDocument::getTitle()
{
	return JSValue::global("document")["title"].as<std::string>();
}

void HtmlDocument::setIcon(const std::string& href)
{
	JSValue element = JSValue::global("document")["head"]["firstElementChild"];
	while (!element.isNull())
	{
		if (element["nodeName"].as<std::string>() == "LINK" && element["rel"].as<std::string>() == "icon")
		{
			element.set("href", href);
			return;
		}
		element = element["nextElementSibling"];
	}

	element = JSValue::global("document").call<JSValue>("createElement", std::string("link"));
	element.set("rel", "icon");
	element.set("href", href);
	JSValue::global("document")["head"].call<void>("appendChild", element);
}

std::string HtmlDocument::getIcon()
{
	JSValue element = JSValue::global("document")["head"]["firstElementChild"];
	while (!element.isNull())
	{
		if (element["nodeName"].as<std::string>() == "LINK" && element["rel"].as<std::string>() == "icon")
		{
			return element["href"].as<std::string>();
		}
		element = element["nextElementSibling"];
	}
	return {};
}
