#include "spawn_teleporter_component.h"
#include "coord_event.h"

void SpawnTeleporterComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		int thisActorID = event.actorID;
		auto getSpawnRequest = std::make_shared<Request>("get_spawn", true, 0, [this, thisActorID]
		(const Event& event)
		{
			const CoordEvent& coordEvent = (const CoordEvent&) event;
			float spawnX = coordEvent.x;
			float spawnY = coordEvent.y;
			auto setCoordsEvent = std::make_shared<CoordEvent>("set_coords", thisActorID, spawnX, spawnY);
			//setCoordsEvent->name = "set_coords";
			localEvents.push_back(setCoordsEvent);
		});
		requests.push_back(getSpawnRequest);
	}
}

bool SpawnTeleporterComponent::hasUpdate(int systemID)
{
	return false;
}

std::string SpawnTeleporterComponent::getName()
{
	return "spawn_teleporter";
}

std::shared_ptr<ComponentUpdate> SpawnTeleporterComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> SpawnTeleporterComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<SpawnTeleporterComponent>();
}

