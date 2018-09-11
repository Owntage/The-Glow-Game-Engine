#include "hp_component.h"
#include "variant_update.h"


void HpComponent::onEvent(const Event& event)
{
	if(event.name == "hp")
	{
		const HpEvent& hpEvent = (const HpEvent&) event;
		currentHp += hpEvent.delta;
		currentHp = std::min(currentHp, maxHp);
		currentDataNumber++;
		std::cout << "current data number: " << currentDataNumber << std::endl;
		if(currentHp <= 0)
		{
			auto deleteEvent = std::make_shared<Event>("delete");
			localEvents.push_back(deleteEvent);
		}
	}
}

bool HpComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		return true;
	}
	return lastSystemApproved[systemID] < currentDataNumber;
}

std::string HpComponent::getName()
{
	return "hp";
}

std::shared_ptr<ComponentUpdate> HpComponent::getUpdate(int syatemID)
{
	auto result = std::make_shared<VariantUpdate>(getName());
	result->number = currentDataNumber;
	result->set("currentHp", currentHp);
	result->set("maxHp", maxHp);
	return result;
}

std::shared_ptr<IComponent> HpComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	//get hp from properties;
	auto result = std::make_shared<HpComponent>();
	result->maxHp = tree.get("hp", 100.0f);
	result->currentHp = result->maxHp;
	return result;
}

