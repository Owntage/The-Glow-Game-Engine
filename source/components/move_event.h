#ifndef MOVE_EVENT_H
#define MOVE_EVENT_H

#include "../event.h"

struct MoveEvent : Event
{
	MoveEvent(bool left, bool right, bool up, bool down, float angle, int actorID): 
		Event("move", false, actorID), 
		left(left), 
		right(right), 
		up(up), 
		down(down),
		angle(angle)
	{} 
	MoveEvent() {}
	bool left, right, up, down;
	float angle;
	bool operator==(const MoveEvent& other)
	{
		return left == other.left && 
			right == other.right && 
			up == other.up && 
			down == other.down &&
			angle == other.angle;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, MoveEvent& e)
	{
		s << (uint8_t) e.left << (uint8_t) e.right << (uint8_t) e.up << (uint8_t) e.down << e.angle;
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, MoveEvent& e)
	{
		s >> (uint8_t&) e.left >> (uint8_t&) e.right >> (uint8_t&) e.up >> (uint8_t&) e.down >> e.angle;
		return s;
	}
};

#endif