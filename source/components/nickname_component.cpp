#include "nickname_component.h"
#include "string_event.h"
#include <cstdlib>
#include <boost/foreach.hpp>

#include "variant_update.h"

void NicknameComponent::onRequest(const Request& request)
{
	if(request.name == "get_nickname")
	{
		
		request.callback(StringEvent("get_nickname", 0, nickname));
	}
}

void NicknameComponent::onEvent(const Event& event)
{
	
	if(event.name == "set_nickname")
	{
		const StringEvent& stringEvent = (const StringEvent&) event;
		nickname = stringEvent.str;
	}
}

bool NicknameComponent::hasUpdate(int systemID)
{
	return lastSystemApproved.find(systemID) == lastSystemApproved.end();
}

std::string NicknameComponent::getName()
{
	return "nickname";
}

std::shared_ptr<ComponentUpdate> NicknameComponent::getUpdate(int systemID)
{
	lastSystemApproved[systemID] = 1;
	auto result = std::make_shared<VariantUpdate>("nickname");
	result->set("nickname", nickname);
	return result;
}

std::shared_ptr<IComponent> NicknameComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<NicknameComponent>();
	result->nickname = tree.get("default_nickname", "Player");
	return result;
}

