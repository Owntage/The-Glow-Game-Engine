#ifndef STRING_EVENT_H
#define STRING_EVENT_H

#include "../event.h"

struct StringEvent : Event
{
	StringEvent(std::string name, int actorID, std::string str) : Event(name, false, actorID),
		str(str) {}
	std::string str;
};

#endif