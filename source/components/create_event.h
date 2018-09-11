#ifndef CREATE_EVENT_H
#define CREATE_EVENT_H

#include "../event.h"
#include <vector>
#include <memory>

struct CreateEvent : Event
{
	CreateEvent() : Event("create") {}
	std::string type;
	std::vector<std::shared_ptr<Event> > events;
};

#endif