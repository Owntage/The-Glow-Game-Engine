#ifndef CHAT_EVENT_H
#define CHAT_EVENT_H

#include "../event.h"
#include <string>

struct ChatEvent : Event
{
	ChatEvent(std::string message, int actorID) : Event("chat", false, actorID), message(message) {}
	ChatEvent() {}
	std::string message;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, ChatEvent& e)
	{
		s << e.message;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, ChatEvent& e)
	{
		s >> e.message;
		return s;
	}
};

#endif