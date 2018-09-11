//
// Created by Owntage on 12/15/2017.
//

#include <algorithm>
#include "sound_component.h"
#include "variant_update.h"
#include <variant/variant_event.h>

using namespace std;

void SoundComponent::onEvent(const Event &event)
{
	if (event.name == "sound")
	{
		VariantEvent& soundEvent = (VariantEvent&) event;
		currentSound = soundEvent.get<std::string>("sound");
		currentDataNumber++;
		cout << "playing sound: " << currentSound << endl;
	}
}

bool SoundComponent::hasUpdate(int systemID)
{
	auto approved = lastSystemApproved.find(systemID);
	if (approved == lastSystemApproved.end())
	{
		return true;
	}
	else
	{
		return approved->second < currentDataNumber && currentDataNumber > 0;
	}
}

std::string SoundComponent::getName()
{
	return "sound";
}

std::shared_ptr<ComponentUpdate> SoundComponent::getUpdate(int systemID)
{
	auto approved = lastSystemApproved.find(systemID);
	auto result = make_shared<VariantUpdate>("sound");
	if (approved == lastSystemApproved.end()) {
		result->set("type", "init");
	} else {
		result->set("type", "sound");
		result->set("sound", currentSound);
	}
	result->number = currentDataNumber;
	return result;
}

std::shared_ptr<IComponent> SoundComponent::loadFromXml(const boost::property_tree::ptree &tree)
{
	return make_shared<SoundComponent>();
}
