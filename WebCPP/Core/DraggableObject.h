#pragma once

#include "WebCPP/Core/EventListener.h"

class Event;
class View;

class DraggableObject
{
public:
	DraggableObject(View* draggable, View* dragHandle = nullptr);
	~DraggableObject();

private:
	void onPointerDown(Event*);
	void onPointerUp(Event*);
	void onPointerMove(Event*);

	View* draggable = nullptr;
	View* dragHandle = nullptr;
	EventListener pointerDownListener;
	EventListener pointerUpListener;
	EventListener pointerMoveListener;
};
