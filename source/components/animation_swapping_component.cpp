#include "animation_swapping_component.h"
#include "move_event.h"
#include "string_event.h"
#include "animation_event.h"
#include "physics_component.h"
#include "coord_event.h"
#include <iostream>
#include <math.h>

#define PI 3.1415926536

void AnimationSwappingComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
	}
	if(event.name == "timer")
	{
		requests.push_back(std::make_shared<Request>("angle", false, thisActorID, 
			[this](const Event& event)
		{
			
			const AngleEvent& angleEvent = (const AngleEvent&) event;
			if(moving)
			{

				localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(runningTexture, true, angle), thisActorID));
			}
			else
			{
				localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(normalTexture, false, angleEvent.angle * 180 / PI), thisActorID));
			}
			localEvents.push_back(std::make_shared<AnimationEvent>(1, LayerState(bodyTexture, true, angleEvent.angle * 180 / PI), thisActorID));
			//std::cout << "request response finished" << std::endl;
		}));
	}
	if(event.name == "move")
	{
		//std::cout << "animation swapping component received move event" << std::endl;
		const MoveEvent& moveEvent = (const MoveEvent&) event;
		if(!(this->moveEvent == moveEvent))
		{
			moving = moveEvent.up || moveEvent.down || moveEvent.left || moveEvent.right;
			
			//here I should count an angle
			float dirX = 0.0f;
			float dirY = 0.0f;
			if(moveEvent.up) dirY -= 1.0f;
			if(moveEvent.down) dirY += 1.0f;
			if(moveEvent.left) dirX -= 1.0f;
			if(moveEvent.right) dirX += 1.0f;


			//float angle;

			if(dirX < 0)
			{
				angle = atan(dirY / dirX) + PI;
			}
			else
			{
				if(dirY < 0)
				{
					angle = atan(dirY / dirX) + 2 * PI;
				}
				else
				{
					angle = atan(dirY / dirX);
				}
			}

			angle = angle / PI * 180.0f;
			

			/*
			if(moving)
			{

				localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(runningTexture, true, angle), event.actorID));
			}
			else
			{
				localEvents.push_back(std::make_shared<AnimationEvent>(0, LayerState(normalTexture, false, moveEvent.angle), event.actorID));
			}
			localEvents.push_back(std::make_shared<AnimationEvent>(1, LayerState(bodyTexture, true, moveEvent.angle), event.actorID));
			*/
			
		}
		
	}
}

bool AnimationSwappingComponent::hasUpdate(int systemID)
{
	return false;
}
std::string AnimationSwappingComponent::getName()
{
	return "animation_swapping";
}

std::shared_ptr<ComponentUpdate> AnimationSwappingComponent::getUpdate(int syatemID)
{
	return std::shared_ptr<ComponentUpdate>();
}

std::shared_ptr<IComponent> AnimationSwappingComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<AnimationSwappingComponent>();
	result->normalTexture = tree.get("normal_texture", "normal_texture");
	result->runningTexture = tree.get("running_texture", "running_texture");
	result->bodyTexture = tree.get("body_texture", "body_texture");
	return result;
}

