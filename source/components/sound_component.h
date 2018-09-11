//
// Created by Owntage on 12/15/2017.
//

#ifndef NETWORK_SHOOTER_PROJECT_SOUND_COMPONENT_H
#define NETWORK_SHOOTER_PROJECT_SOUND_COMPONENT_H

#include "icomponent.h"

struct SoundComponent : IComponent
{
	SoundComponent()
	{
		std::cout << "SoundComponent created" << std::endl;
	}
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate>  getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	std::string currentSound;
};

#endif //NETWORK_SHOOTER_PROJECT_SOUND_COMPONENT_H
