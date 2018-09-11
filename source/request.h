#ifndef REQUEST_H
#define REQUEST_H

#include <functional>
#include "event.h"

struct Request
{
	Request(std::string name, bool global, int actorID, std::function<void(const Event&)> callback) :
		name(name),
		global(global),
		actorID(actorID),
		callback(callback)
	{}
	std::string name;
	bool global;
	int actorID;
	std::function<void(const Event&)> callback;
};

#endif