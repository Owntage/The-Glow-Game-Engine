#include "damage_dealer_component.h"
#include "hp_component.h"
#include <boost/foreach.hpp>

void DamageDealerComponent::onEvent(const Event& event)
{
	if(event.name == "damage")
	{
		std::cout << "damage dealer received damage" << std::endl;
		const DamageEvent& damageEvent = (const DamageEvent&) event;
		float multiplier = 1.0f;
		std::cout << "damage: " << damageEvent.damage << std::endl;
		if(multipliers.find(damageEvent.bulletType) != multipliers.end())
		{
			multiplier = multipliers[damageEvent.bulletType];
		}
		auto hpEvent = std::make_shared<HpEvent>(-damageEvent.damage * multiplier, damageEvent.actorID);
		localEvents.push_back(hpEvent);
	}
}

bool DamageDealerComponent::hasUpdate(int systemID)
{
	return false;
}

std::string DamageDealerComponent::getName()
{
	return "damage_dealer";
}

std::shared_ptr<ComponentUpdate> DamageDealerComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> DamageDealerComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<DamageDealerComponent>();
	BOOST_FOREACH(auto& v, tree)
	{
		if(v.first == "multiplier")
		{
			std::string type = v.second.get<std::string>("type");
			float value = v.second.get<float>("value");
			result->multipliers[type] = value;
		}
	}
	return result;
}

