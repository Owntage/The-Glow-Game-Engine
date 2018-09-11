#ifndef DEATHMATCH_COMPONENT_H
#define DEATHMATCH_COMPONENT_H

#include "icomponent.h"
#include "frag_count_component.h"
#include <vector>
#include <utility>

struct DeathmatchComponent : IComponent
{
	DeathmatchComponent() :
		currentTime(0.0f)
	{}
	void onRequest(const Request& request);
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	int totalRequests;
	int thisActorID;
	int requestsHandled;
	std::vector<std::pair<std::string, int> > aliveActors;
	std::vector<std::pair<std::string, int> > tempAliveActors;
	std::vector<std::pair<float, float> > spawns;
	float roundTime;
	float currentTime;
};

#endif