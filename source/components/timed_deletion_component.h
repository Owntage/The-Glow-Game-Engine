#ifndef TIMED_DELETION_COMPONENT_H
#define TIMED_DELETION_COMPONENT_H

#include "icomponent.h"

struct TimedDeletionComponent : IComponent
{
	TimedDeletionComponent() : 
		currentTime(0.0f)
	{}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	float currentTime;
	float time;
};

#endif