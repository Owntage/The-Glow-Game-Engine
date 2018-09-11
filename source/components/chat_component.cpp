#include "chat_component.h"
#include <iostream>
#include "string_event.h"
#include "variant_update.h"

std::vector<std::string> ChatComponent::chatHistory;

void ChatComponent::onEvent(const Event& event)
{
	if(event.name == "chat")
	{
		const ChatEvent& chatEvent = static_cast<const ChatEvent&>(event);
		std::string message = chatEvent.message;
		//chatHistory.push_back(chatEvent.message);
		auto request = std::make_shared<Request>("get_nickname", false, event.actorID, [message, this]
		(const Event& event)
		{
			const StringEvent& stringEvent = (const StringEvent&) event;
			chatHistory.push_back(stringEvent.str + ": " + message);
		});
		requests.push_back(request);
		//std::cout << "chat component received a message: " << chatEvent.message << std::endl;
	}
}

bool ChatComponent::hasUpdate(int systemID)
{
	
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		lastSystemApproved[systemID] = 0;
		return true;
	}
	return lastSystemApproved[systemID] != chatHistory.size();
}

std::string ChatComponent::getName()
{
	return "chat";
}

std::shared_ptr<ComponentUpdate> ChatComponent::getUpdate(int systemID)
{
	std::shared_ptr<VariantUpdate> result = std::make_shared<VariantUpdate>(getName());
	result->number = chatHistory.size();
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		lastSystemApproved[systemID] = 0;
	}

	int rangeBegin = lastSystemApproved[systemID];
	int rangeEnd = chatHistory.size() - 1;

	result->set("rangeBegin", rangeBegin);
	result->set("rangeEnd", rangeEnd);
	std::vector<std::string> messages;
	for(int i = rangeBegin; i <= rangeEnd; i++)
	{
		messages.push_back(chatHistory[i]);
	}
	result->setVector("messages", messages);
	return result;
}

std::shared_ptr<IComponent> ChatComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<ChatComponent>();
}
//todo add to factory

