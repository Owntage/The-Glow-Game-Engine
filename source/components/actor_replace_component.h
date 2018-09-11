#ifndef ACTOR_REPLACE_COMPONENT_H
#define ACTOR_REPLACE_COMPONENT_H

#include "icomponent.h"

struct ActorReplaceComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string replaceWith;
};

#endif