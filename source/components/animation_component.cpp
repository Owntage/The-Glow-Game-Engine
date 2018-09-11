#include "animation_component.h"
#include "animation_event.h"
#include "animation_update.h"
#include "physics_component.h"
#include "boost/foreach.hpp"

void AnimationComponent::onEvent(const Event& event)
{
	if(event.name == "animation")
	{
		AnimationEvent& animationEvent = (AnimationEvent&) event;
		if(states.find(animationEvent.layerState.state) != states.end() && animationEvent.layerNumber < currentLayerStates.size())
		{
			if (currentLayerStates[animationEvent.layerNumber] != animationEvent.layerState)
			{
				currentLayerStates[animationEvent.layerNumber] = animationEvent.layerState;
				currentDataNumber++;
			}
		}
		
	}
	if(event.name == "animation_angle")
	{
		const AnimationAngleEvent& animationAngleEvent = (const AnimationAngleEvent&) event;
		if(animationAngleEvent.layer < currentLayerStates.size())
		{
			currentLayerStates[animationAngleEvent.layer].angle = animationAngleEvent.angle;
		}
		
	}
	
}

bool AnimationComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end()) return true;
	return lastSystemApproved[systemID] != currentDataNumber;
}

std::shared_ptr<ComponentUpdate> AnimationComponent::getUpdate(int systemID)
{
	std::shared_ptr<AnimationUpdate> result = std::make_shared<AnimationUpdate>(currentLayerStates, currentDataNumber);
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		for(auto it = states.begin(); it != states.end(); it++)
		{
			result->states.push_back(it->second);
		}
	}
	return result;
}

std::shared_ptr<IComponent> AnimationComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	std::shared_ptr<AnimationComponent> result = std::make_shared<AnimationComponent>();
	bool first = true;
	std::string firstState;
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "state")
		{
			AnimationState animationState;
			animationState.delay = 0.3f;
			animationState.stateName = "default";
			
			BOOST_FOREACH(auto& state_v, v.second)
			{
				if(state_v.first == "name")
				{
					animationState.stateName = state_v.second.get_value<std::string>();
				}
				if(state_v.first == "delay")
				{
					animationState.delay = state_v.second.get_value<float>();
				}
				if(state_v.first == "image")
				{
					animationState.images.push_back(state_v.second.get_value<std::string>());
				}
			}
			if(first)
			{
				first = false;
				firstState = animationState.stateName;
			}
			result->states[animationState.stateName] = animationState;
		}
	}

	int layerCount = tree.get("layer_count", 1);
	result->currentLayerStates.resize(layerCount, LayerState("", false, 0.0f));
	result->currentLayerStates[0].isDrawing = true;
	result->currentLayerStates[0].state = firstState;

	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "layer")
		{
			int layer_number;
			bool is_active;
			float angle;
			std::string state;
			BOOST_FOREACH(auto& layer_v, v.second)
			{
				if(layer_v.first == "layer_number")
				{
					layer_number = layer_v.second.get_value<int>();
				}
				if(layer_v.first == "is_active")
				{
					std::string is_active_str = layer_v.second.get_value<std::string>();
					is_active = is_active_str == "true";
				}
				if(layer_v.first == "state")
				{
					state = layer_v.second.get_value<std::string>();
				}
				if(layer_v.first == "angle")
				{
					angle = layer_v.second.get_value<float>();
				}
			}
			result->currentLayerStates[layer_number] = LayerState(state, is_active, angle);
		}
	}

	return result;
}