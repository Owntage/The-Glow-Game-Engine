#include "frag_count_component.h"
#include "damage_dealer_component.h"

std::map<int, int> FragCountComponent::frags;

void FragCountComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id" && frags.find(event.actorID) == frags.end())
	{
		frags[event.actorID] = 0;
	}
	if(event.name == "damage")
	{
		const DamageEvent& damageEvent = (const DamageEvent&) event;
		killers[damageEvent.shotBy] = 0;
	}
	if(event.name == "timer")
	{
		for(auto it = killers.begin(); it != killers.end();)
		{
			it->second++;
			if(it->second > 60)
			{
				killers.erase(it++);
			}
			else
			{
				it++;
			}
		}
	}
	if(event.name == "death")
	{
		for(auto it = killers.begin(); it != killers.end(); it++)
		{
			if(frags.find(it->first) == frags.end())
			{
				frags[it->first] = 0;
			}
			frags[it->first]++;
			
		}
	}
}

bool FragCountComponent::hasUpdate(int systemID)
{
	return false;
}

std::string FragCountComponent::getName()
{
	return "frag_count";
}

std::shared_ptr<ComponentUpdate> FragCountComponent::getUpdate(int syatemID)
{
	std::shared_ptr<ComponentUpdate> result;
	return result;
}

std::shared_ptr<IComponent> FragCountComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<FragCountComponent>();
	return result;
}

