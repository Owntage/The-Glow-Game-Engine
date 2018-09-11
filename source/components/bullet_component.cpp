#include "bullet_component.h"
#include "damage_dealer_component.h"
#include "physics_component.h"

void BulletComponent::onEvent(const Event& event)
{
	if(event.name == "damage")
	{
		damageEvent = (const DamageEvent&) event;
	}
	if(event.name == "physics")
	{
		const PhysicsEvent& physicsEvent = (const PhysicsEvent&) event;
		if(!physicsEvent.release)
		{
			auto newDamageEvent = std::make_shared<DamageEvent>(damageEvent);
			newDamageEvent->actorID = physicsEvent.otherActorID;
			localEvents.push_back(newDamageEvent);
		}
	}
}

bool BulletComponent::hasUpdate(int systemID)
{
	return false;
}

std::string BulletComponent::getName()
{
	return "bullet";
}

std::shared_ptr<ComponentUpdate> BulletComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> BulletComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<BulletComponent>();
}

