#include "actor_replace_component.h"
#include "string_event.h"


void ActorReplaceComponent::onEvent(const Event& event)
{
	if(event.name == "death")
	{
		auto replaceEvent = std::make_shared<StringEvent>("replace", 0, replaceWith);
		localEvents.push_back(replaceEvent);
	}
}

bool ActorReplaceComponent::hasUpdate(int systemID)
{
	return false;
}

std::string ActorReplaceComponent::getName()
{
	return "actor_replace";
}

std::shared_ptr<ComponentUpdate> ActorReplaceComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> ActorReplaceComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<ActorReplaceComponent>();
	result->replaceWith = tree.get<std::string>("replace_with");
	return result;
}

