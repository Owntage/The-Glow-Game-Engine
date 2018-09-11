#ifndef COMPONENT_UPDATE_H
#define COMPONENT_UPDATE_H

#include <string>

struct ComponentUpdate
{
	ComponentUpdate(std::string name): name(name) {}
	ComponentUpdate() {}
	virtual ~ComponentUpdate() {}
	std::string name;
	int number;
	int actorID;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, ComponentUpdate& componentUpdate)
	{
		s << componentUpdate.name << componentUpdate.number << componentUpdate.actorID;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, ComponentUpdate& componentUpdate)
	{
		s >> componentUpdate.name >> componentUpdate.number >> componentUpdate.actorID;
		return s;
	}
};

#endif