#include "timed_deletion_component.h"

void TimedDeletionComponent::onEvent(const Event& event)
{
	if(event.name == "timer")
	{
		currentTime += 1.0 / 60.0f;
		if(currentTime >= time)
		{
			localEvents.push_back(std::make_shared<Event>("delete"));
		}
	}
}

bool TimedDeletionComponent::hasUpdate(int systemID)
{
	return false;
}

std::string TimedDeletionComponent::getName()
{
	return "timed_deletion";
}

std::shared_ptr<ComponentUpdate> TimedDeletionComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> TimedDeletionComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<TimedDeletionComponent>();
	result->time = tree.get("time", 3.0f);
	return result;
}
