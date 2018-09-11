#include "contact_deletion_component.h"
#include "physics_component.h"
#include <boost/foreach.hpp>


void ContactDeletionComponent::onEvent(const Event& event)
{
	if(event.name == "physics")
	{
		const PhysicsEvent& physicsEvent = (const PhysicsEvent&) event;
		if(deleterTypes.find(physicsEvent.otherType) != deleterTypes.end() && !physicsEvent.release || allFlag)
		{
			localEvents.push_back(std::make_shared<Event>("delete"));
		}
		
		
	}
}

bool ContactDeletionComponent::hasUpdate(int systemID)
{
	return false;
}

std::string ContactDeletionComponent::getName()
{
	return "contact_deletion";
}

std::shared_ptr<ComponentUpdate> ContactDeletionComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> ContactDeletionComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<ContactDeletionComponent>();
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "deleter_type")
		{
			result->deleterTypes.insert(v.second.get_value<std::string>());
			if(v.second.get_value<std::string>() == "all")
			{
				result->allFlag = true;
				
			}
		}
	}
	return result;
}

