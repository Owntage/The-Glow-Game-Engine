#ifndef SPAWNER_COMPONENT_H
#define SPAWNER_COMPONENT_H

#include "icomponent.h"
#include <vector>
#include <string>

struct SpawnerComponent : IComponent
{
	SpawnerComponent();
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	float time;
	float currentTime;
	int currentType;
	int thisActorID;
	std::string spawnMode;
	std::vector<std::string> spawnerTypes;
};

#endif