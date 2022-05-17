#pragma once
#include<vector>

#include "EventHandler.h"

class EventSystem
{
	std::vector<EventHandler> eventHandlers;
public:
	EventSystem();
	void AddEvent(EventHandler eventHandler);
	void ProcessEvent();
};
