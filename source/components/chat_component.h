#ifndef CHAT_COMPONENT_H
#define CHAT_COMPONENT_H

#include "icomponent.h"
#include "chat_event.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

struct ChatComponent : IComponent
{
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate>  getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);

private:
	static std::vector<std::string> chatHistory;
};



#endif