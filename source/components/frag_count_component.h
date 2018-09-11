#ifndef FRAG_COUNT_COMPONENT_H
#define FRAG_COUNT_COMPONENT_H

#include "icomponent.h"
#include <vector>
#include <map>



struct FragCountComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
	static std::map<int, int> frags;
private:
	std::map<int, int> killers;
	
};

#endif