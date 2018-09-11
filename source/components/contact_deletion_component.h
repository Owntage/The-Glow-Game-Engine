#ifndef CONTACT_DELETION_COMPONENT_H
#define CONTACT_DELETION_COMPONENT_H

#include "icomponent.h"
#include <set>
#include <string>

struct ContactDeletionComponent : IComponent
{
	ContactDeletionComponent() :
		allFlag(false) {}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::set<std::string> deleterTypes;
	bool allFlag;
};

#endif