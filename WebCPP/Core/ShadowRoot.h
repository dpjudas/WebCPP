#pragma once

#include <functional>
#include <vector>
#include "JSValue.h"

class Element;

class ShadowRoot
{
public:
	ShadowRoot(JSValue handle);
	~ShadowRoot();

	void insertBefore(Element* newElement, Element* insertPoint);
	void appendChild(Element* newElement);
	void removeChild(Element* childElement);

	static void setComponentStyleSheet(const std::string& value);

	JSValue handle;
};
