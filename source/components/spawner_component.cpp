#include "spawner_component.h"
#include "coord_event.h"
#include "create_event.h"
#include "request.h"
#include <boost/foreach.hpp>

SpawnerComponent::SpawnerComponent() :
	currentTime(0.0f),
	currentType(0)
{}

void SpawnerComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
	}
	if(event.name == "timer")
	{
		currentTime += 1.0f / 60.0f;
		if(currentTime >= time)
		{
			std::cout << "spawning..." << std::endl;
			currentTime = 0.0f;
			if(spawnerTypes.size() > 0)
			{
				int index;
				if(spawnMode == "random")
				{
					index = rand() % spawnerTypes.size();
				}
				else
				{
					index = currentType;
					currentType = (currentType + 1) % spawnerTypes.size();
				}
				
				requests.push_back(std::make_shared<Request>("coords", false, thisActorID, 
					[this, index](const Event& event)
				{
					const CoordEvent& thisCoordEvent = (const CoordEvent&) event;
					auto createEvent = std::make_shared<CreateEvent>();
					createEvent->actorID = thisActorID;
					createEvent->type = spawnerTypes[index];
					createEvent->events.push_back(std::make_shared<CoordEvent>("set_coords", 0, thisCoordEvent.x, thisCoordEvent.y));
					float angle = rand() % 100;
					angle /= 100.0f;
					angle *= 6.28;
					float s_x = 10 * cos(angle);
					float s_y = 10 * sin(angle);
					createEvent->events.push_back(std::make_shared<CoordEvent>("set_speed", 0, s_x, s_y));
					localEvents.push_back(createEvent);
				}));
				//add events and add it in vector
			}
		}
	}
}

bool SpawnerComponent::hasUpdate(int systemID)
{
	return false;
}

std::string SpawnerComponent::getName()
{
	return "spawner";
}

std::shared_ptr<ComponentUpdate> SpawnerComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> SpawnerComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<SpawnerComponent>();
	result->time = tree.get("time", 1.0f);
	result->spawnMode = tree.get("spawn_mode", "sequent");
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "type")
		{
			result->spawnerTypes.push_back(v.second.get_value<std::string>());
		}
	}

	return result;
}

