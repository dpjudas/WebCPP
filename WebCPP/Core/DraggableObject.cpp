#include "WebCPP/Core/DraggableObject.h"
#include "WebCPP/Core/Element.h"
#include "WebCPP/Core/Event.h"
#include "WebCPP/Core/View.h"
#include <format>

DraggableObject::DraggableObject(View* draggable, View* draggableHandle)
	:
	draggable(draggable),
	pointerDownListener("pointerdown", std::bind(&DraggableObject::onPointerDown, this, std::placeholders::_1)),
	pointerUpListener("pointerup", std::bind(&DraggableObject::onPointerUp, this, std::placeholders::_1)),
	pointerMoveListener("pointermove", std::bind(&DraggableObject::onPointerMove, this, std::placeholders::_1))
{
	dragHandle = draggableHandle != nullptr ? draggableHandle : draggable;
	dragHandle->addClass("drag-handle");
	pointerDownListener.addEventListener(dragHandle->element.get());
	pointerUpListener.addEventListener(dragHandle->element.get());
}

DraggableObject::~DraggableObject()
{
}

void DraggableObject::onPointerDown(Event* e)
{
	e->stopPropagation();
	e->preventDefault();
	dragHandle->element->setPointerCapture(e->pointerId());

	draggable->element->setStyle("position", "absolute");
	dragHandle->addClass("dragging");
	pointerMoveListener.addEventListener(dragHandle->element.get());
}

void DraggableObject::onPointerUp(Event* e)
{
	e->stopPropagation();
	e->preventDefault();
	dragHandle->element->releasePointerCapture(e->pointerId());

	dragHandle->removeClass("dragging");
	pointerMoveListener.removeEventListener();
}

void DraggableObject::onPointerMove(Event* e)
{
	e->stopPropagation();
	e->preventDefault();

	auto rect = draggable->element->getBoundingClientRect();
	auto newLeft = rect.x + e->handle["movementX"].as<double>();
	auto newTop = rect.y + e->handle["movementY"].as<double>();
	draggable->element->setStyle("left", std::to_string(newLeft) + "px");
	draggable->element->setStyle("top", std::to_string(newTop) + "px");
}
