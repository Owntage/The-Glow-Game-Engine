#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "icomponent.h"
#include <map>

struct MoveComponent : IComponent
{
	void onEvent(const Event& event);
	void onRequest(const Request& request);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int systemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	bool movingLeft;
	bool movingRight;
	bool movingUp;
	bool movingDown;
	float speed;
	float x, y;
	//std::map<int, MoveUpdate> systemUpdates;
};

#endif