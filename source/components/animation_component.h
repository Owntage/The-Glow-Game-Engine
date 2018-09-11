#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "icomponent.h"
#include <vector>
#include <string>
#include <map>

struct AnimationState
{
	std::vector<std::string> images;
	float delay;
	std::string stateName;

	AnimationState() :
			delay(0)
	{}
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, AnimationState& a)
	{
		s << a.stateName << a.delay << (int) a.images.size();
		for(int i = 0; i < a.images.size(); i++)
		{
			s << a.images[i];
		}
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, AnimationState& a)
	{
		s >> a.stateName >> a.delay;
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			std::string temp;
			s >> temp;
			a.images.push_back(temp);
		}
		return s;
	}
};

struct LayerState
{
	LayerState(std::string state, bool isDrawing, float angle) :
		state(state),
		isDrawing(isDrawing),
		angle(angle)
	{}
	LayerState() :
			state(0),
			isDrawing(false),
			angle(0)
	{}
	bool operator==(const LayerState& other)
	{
		return isDrawing == other.isDrawing && state == other.state &&
			angle == other.angle;
	}
	bool operator!=(const LayerState& other)
	{
		return !(*this == other);
	}
	std::string state;
	bool isDrawing;
	float angle;
};

struct AnimationAngleEvent : Event
{
	AnimationAngleEvent(float angle, int layer, int actorID) :
		Event("animation_angle", false, actorID),
		angle(angle),
		layer(layer),
		actorID(actorID)
	{}
	float angle;
	int layer;
	int actorID;
};

struct AnimationComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName() {return "animation"; }
	std::shared_ptr<ComponentUpdate> getUpdate(int systemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::map<std::string, AnimationState> states;
	std::vector<LayerState> currentLayerStates;
};

#endif