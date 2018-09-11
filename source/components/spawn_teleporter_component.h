#ifndef SPAWNER_TELEPORTER_COMPONENT_H
#define SPAWNER_TELEPORTER_COMPONENT_H

#include "icomponent.h"
#include "physics_component.h"

struct SpawnTeleporterComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);

};

#endif