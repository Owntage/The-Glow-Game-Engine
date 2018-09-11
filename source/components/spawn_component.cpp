#include "spawn_component.h"
#include "physics_component.h"
#include "coord_event.h"


void SpawnComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
		auto coordRequest = std::make_shared<Request>("coords", false, event.actorID, [this]
		(const Event& event)
		{
			const CoordEvent& coordEvent = (const CoordEvent&) event;
			spawnX = coordEvent.x;
			spawnY = coordEvent.y;
			hasSpawnCoords = true;
		});
		requests.push_back(coordRequest);
	}
	if(event.name == "timer")
	{
		if(hasSpawnCoords)
		{
			if(!hasGamemodeID)
			{
				auto gamemodeRequest = std::make_shared<Request>("get_gamemode_id", true, 0, [this]
				(const Event& event)
				{
					hasGamemodeID = true;
					gameModeID = event.actorID;
				});
				requests.push_back(gamemodeRequest);
			}
			else
			{
				if(!spawnCoordsSet)
				{
					auto spawnEvent = std::make_shared<CoordEvent>("set_spawn", gameModeID, spawnX, spawnY);
					localEvents.push_back(spawnEvent);
					spawnCoordsSet = true;
				}
			}
		}
	}
}

bool SpawnComponent::hasUpdate(int systemID)
{
	return false;
}

std::string SpawnComponent::getName()
{
	return "spawn";
}

std::shared_ptr<ComponentUpdate> SpawnComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> SpawnComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<SpawnComponent>();
}

