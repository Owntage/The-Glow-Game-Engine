#ifndef NICKNAME_COMPONENT_H
#define NICKNAME_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"
#include <map>
#include <set>
#include <utility>

struct NicknameComponent : IComponent
{
	void onRequest(const Request& request);
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string nickname;
};

#endif