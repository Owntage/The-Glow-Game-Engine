#ifndef EVENT_H
#define EVENT_H

#include <string>

struct Event
{
	Event(std::string name, bool global, int actorID): name(name), global(global), actorID(actorID) {}
	Event(std::string name) : name(name), global(true), actorID(0) {}
	Event() {}
	virtual ~Event() {}
	std::string name;
	bool global;
	int actorID;
	int number;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, Event& e)
	{
		s << e.name << (uint8_t&) e.global << e.actorID << e.number;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, Event& e)
	{
		s >> e.name >> (uint8_t&) e.global >> e.actorID >> e.number;
		return s;
	}
};

#endif