#pragma once

#include "JSCallback.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

class Element;
class Event;

class EventListener
{
public:
	explicit EventListener(std::string name, std::function<void(Event*)> handler);
	explicit EventListener(std::string name, std::function<void(Event*)> handler, Element* element);
	EventListener(const EventListener&) = delete;
	EventListener(EventListener&&) = default;
	EventListener& operator=(const EventListener&) = delete;
	EventListener& operator=(EventListener&&) = default;
	~EventListener();

	void addEventListener(Element* element);
	void removeEventListener();

private:
	std::string name;
	Element* listener = nullptr;
	std::function<void(Event*)> handler;
	std::unique_ptr<JSCallback> callback;
};
