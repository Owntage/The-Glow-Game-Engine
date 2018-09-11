#ifndef HP_COMPONENT_H
#define HP_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"

struct HpEvent : Event
{
	HpEvent(float delta, int actorID) : 
		Event("hp", false, actorID),
		delta(delta)
	{}
	float delta;
};

struct HpComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	float currentHp;
	float maxHp;
};

#endif