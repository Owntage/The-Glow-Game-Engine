#ifndef SPAWN_COMPONENT_H
#define SPAWN_COMPONENT_H

#include "icomponent.h"
#include <functional>

struct SpawnComponent : IComponent
{
	SpawnComponent() :
		hasSpawnCoords(false),
		hasGamemodeID(false),
		spawnCoordsSet(false)
	{}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	float spawnX;
	float spawnY;
	bool hasSpawnCoords;
	int thisActorID;
	bool hasGamemodeID;
	int gameModeID;
	bool spawnCoordsSet;
};


#endif