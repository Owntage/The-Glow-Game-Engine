#ifndef BULLET_COMPONENT_H
#define BULLET_COMPONENT_H

#include "icomponent.h"
#include "damage_dealer_component.h"

struct BulletComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	DamageEvent damageEvent;
};

#endif