#ifndef ANIMATION_SWAPPING_COMPONENT_H
#define ANIMATION_SWAPPING_COMPONENT_H

#include "icomponent.h"
#include "move_event.h"

struct AnimationSwappingComponent : IComponent
{
	AnimationSwappingComponent() :
		moving(false),
		angle(0.0f)
	{}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	int thisActorID;
	bool moving;
	float angle;
	std::string runningTexture;
	std::string normalTexture;
	std::string bodyTexture;
	MoveEvent moveEvent;
};

#endif