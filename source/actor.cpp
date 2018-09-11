#include "actor.h"

bool Actor::hasUpdates(int systemID)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->hasUpdate(systemID)) return true;
	}
	return false;
}

bool Actor::hasEvents()
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->globalEvents.size() > 0) return true;
	}
	return false;
}

ActorUpdate Actor::getUpdates(int systemID)
{
	ActorUpdate result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->hasUpdate(systemID))
		{
			result.updates.push_back((*it)->getUpdate(systemID));
		}
	}
	return result;
}



std::vector<std::shared_ptr<Event> > Actor::getGlobalEvents()
{
	std::vector<std::shared_ptr<Event> > result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		while((*it)->globalEvents.size() > 0)
		{
			result.push_back((*it)->globalEvents.back());
			(*it)->globalEvents.pop_back();
		}
	}
	return result;
}

std::vector<std::shared_ptr<Event> > Actor::getLocalEvents()
{
	std::vector<std::shared_ptr<Event> > result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		while((*it)->localEvents.size() > 0)
		{
			result.push_back((*it)->localEvents.back());
			(*it)->localEvents.pop_back();
		}
	}
	return result;
}

std::vector<std::shared_ptr<Request> > Actor::getRequests()
{
	std::vector<std::shared_ptr<Request> > result;
	for(auto it = components.begin(); it != components.end(); it++)
	{
		while((*it)->requests.size() > 0)
		{
			result.push_back((*it)->requests.back());
			(*it)->requests.pop_back();
		}
	}
	return result;
}

void Actor::onEvent(const Event& event)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		(*it)->onEvent(event);
	}
}

void Actor::onRequest(const Request& request)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		(*it)->onRequest(request);
	}
}

void Actor::approve(std::string component, int systemID, int number)
{
	for(auto it = components.begin(); it != components.end(); it++)
	{
		if((*it)->getName() == component)  (*it)->approve(systemID, number);
	}
}





