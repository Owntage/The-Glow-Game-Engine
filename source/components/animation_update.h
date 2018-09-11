#ifndef ANIMATION_UPDATE_H
#define ANIMATION_UPDATE_H

#include "component_update.h"
#include <string>
#include <vector>
#include "animation_component.h"
#include <stdint.h>

struct AnimationUpdate : ComponentUpdate
{
	AnimationUpdate(std::vector<LayerState> currentLayerStates, int number) : 
		ComponentUpdate("animation"), 
		currentLayerStates(currentLayerStates)
	{
		this->number = number;
	}
	AnimationUpdate() {}
	std::vector<LayerState> currentLayerStates;
	std::vector<AnimationState> states;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, AnimationUpdate& u)
	{
		s << (int) u.states.size();
		for(int i = 0; i < u.states.size(); i++)
		{
			s << u.states[i];
		}
		s << (int) u.currentLayerStates.size();
		for(int i = 0; i < u.currentLayerStates.size(); i++)
		{
			s << (uint8_t) u.currentLayerStates[i].isDrawing;
			s << u.currentLayerStates[i].state;
			s << u.currentLayerStates[i].angle;
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, AnimationUpdate& u)
	{
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			AnimationState temp;
			s >> temp;
			u.states.push_back(temp);
		}
		s >> size;
		for(int i = 0; i < size; i++)
		{
			bool active;
			std::string state;
			float angle;
			s >> (uint8_t&) active >> state >> angle;
			u.currentLayerStates.push_back(LayerState(state, active, angle));
		}
		return s;
	}
};

#endif