#ifndef DAMAGE_DEALER_COMPONENT_H
#define DAMAGE_DEALER_COMPONENT_H

#include "icomponent.h"
#include <map>

struct DamageEvent : Event
{
	DamageEvent(float damage, std::string bulletType, int shotBy, int actorID) : 
		Event("damage", false, actorID),
		damage(damage),
		bulletType(bulletType),
		shotBy(shotBy)
	{}
	DamageEvent() {}
	float damage;
	std::string bulletType;
	int shotBy;
};

struct DamageDealerComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::map<std::string, float> multipliers;
};

#endif