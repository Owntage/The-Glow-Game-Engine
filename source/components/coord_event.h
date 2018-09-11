#ifndef COORD_EVENT_H
#define COORD_EVENT_H

#include "../event.h"

struct CoordEvent : Event
{
	CoordEvent(std::string name, int actorID, float x, float y) : Event(name, false, actorID),
		x(x), y(y) {}
	float x;
	float y;
};

#endif