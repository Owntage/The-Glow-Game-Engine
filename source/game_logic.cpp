#include "game_logic.h"
#include "delete_update.h"
#include "components/create_event.h"
#include "components/string_event.h"
#include <iostream>

void GameLogic::thrownEventHandler(std::vector<std::shared_ptr<Event> >& events, bool global, int actorID)
{
	while(events.size() > 0)
	{
		auto event = events.back();
		event->global = global;
		
		if(event->name == "delete")
		{
			
			actorsMarkedToDelete.insert(actorID);
			events.pop_back();
		}
		else
		{
			if(event->name == "create")
			{
				createEvents.push_back(event);
				events.pop_back();
			}
			else
			{
				if(event->name == "replace")
				{
					const StringEvent& stringEvent = (const StringEvent&) *event;
					replaceEvents.push_back(RecreateData(stringEvent.str, actorID, actors[actorID]->getCurrentNumber()));
					events.pop_back();
				}
				else
				{
					onEvent(*event, false);
					events.pop_back();
				}
				
			}
			
		}
		
	}
}

void GameLogic::onEvent(const Event& event)
{
	if(event.global || actors.find(event.actorID) != actors.end())
	{
		onEvent(event, true);
	}
	
}

void GameLogic::onEvent(const Event& event, bool shouldDelete)
{
	if(event.global)
	{
		for(auto it = actors.begin(); it != actors.end(); it++)
		{
			it->second->onEvent(event);
			auto globalEvents = it->second->getGlobalEvents();
			thrownEventHandler(globalEvents, true, it->first);
			auto localEvents = it->second->getLocalEvents();
			thrownEventHandler(localEvents, false, it->first);
		}
	}
	else
	{
		actors[event.actorID]->onEvent(event);
		auto globalEvents = actors[event.actorID]->getGlobalEvents();
		thrownEventHandler(globalEvents, true, event.actorID);
		auto localEvents = actors[event.actorID]->getLocalEvents();
		thrownEventHandler(localEvents, false, event.actorID);
	}
	if(shouldDelete)
	{

		//requests

		for(auto it = actors.begin(); it != actors.end(); it++)
		{
			auto requests = it->second->getRequests();
			while(requests.size() > 0)
			{
				if(requests.back()->global)
				{
					for(auto it2 = actors.begin(); it2 != actors.end(); it2++)
					{
						it2->second->onRequest(*requests.back());
					}
				}
				else
				{
					if(actors.find(requests.back()->actorID) != actors.end())
					{
						actors[requests.back()->actorID]->onRequest(*requests.back());
					}
					else
					{
						(*requests.back()).callback(Event("not_exists"));
					}
				}
				
				requests.pop_back();

			}
		}

		//death events

		for(auto it = actorsMarkedToDelete.begin(); it != actorsMarkedToDelete.end(); it++)
		{
			onEvent(Event("death", false, *it), false);
		}

		//creating new actors
		
		while(createEvents.size() > 0)
		{
			
			CreateEvent& createEvent = (CreateEvent&) *createEvents.back();
			int createdActor = createActor(createEvent.type);
			
			while(createEvent.events.size() > 0)
			{
				createEvent.events.back()->global = false;
				createEvent.events.back()->actorID = createdActor;
				onEvent(*createEvent.events.back(), false);
				createEvent.events.pop_back();
			}
			createEvents.pop_back();
		}
		
		//destroying actors, marked for deletion

		for(auto it = actorsMarkedToDelete.begin(); it != actorsMarkedToDelete.end(); it++)
		{
			destroyActor(*it);
		}
		actorsMarkedToDelete.clear();

		//replacing actors:
		while(replaceEvents.size() > 0)
		{
			actors[replaceEvents.back().actorID] = actorFactory.createActor(replaceEvents.back().type);
			actors[replaceEvents.back().actorID]->setCurrentNumber(replaceEvents.back().dataNumber);
			onEvent(Event("actor_id", false, replaceEvents.back().actorID), false);
			replaceEvents.pop_back();
		}
	}
}

void GameLogic::approve(int actorID, std::string component, int systemID, int number)
{
	if(component == "delete")
	{
		deletedActorApproves[systemID] = std::max(deletedActorApproves[systemID], number);
		return;
	}
	if(actors.find(actorID) != actors.end())
	{
		actors[actorID]->approve(component, systemID, number);
	}
}

std::vector<std::shared_ptr<ActorUpdate> > GameLogic::getUpdates(int systemID)
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		if(it->second->hasUpdates(systemID))
		{
			result.push_back(std::make_shared<ActorUpdate>(it->second->getUpdates(systemID)));
			result.back()->actorID = it->first;
			result.back()->actor = it->second;
			for(auto component_update = result.back()->updates.begin(); component_update != result.back()->updates.end(); component_update++)
			{
				(*component_update)->actorID = it->first;
			}
		}
		
	}

	if(deletedActorApproves[systemID] != deletedActors.size() - 1)
	{
		auto deleteUpdate = std::make_shared<DeleteUpdate>(deletedActors.size() - 1);
		for(int i = deletedActorApproves[systemID] + 1; i < deletedActors.size(); i++)
		{
			deleteUpdate->deletedActors.push_back(deletedActors[i]);
		}
		deleteUpdate->actorID = -1;
		auto deleteActorUpdate = std::make_shared<ActorUpdate>();
		deleteActorUpdate->actorID = -1;
		deleteActorUpdate->updates.push_back(deleteUpdate);
		result.push_back(deleteActorUpdate);
	}
	return result;
}

std::vector<std::shared_ptr<ActorUpdate> > GameLogic::getUpdates(int systemID, float x, float y, float updateRadius)
{
	std::vector<std::shared_ptr<ActorUpdate> > result;
	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		if(it->second->hasUpdates(systemID))
		{
			if(it->second->hasCoords && sqrt(pow(it->second->x - x, 2) + pow(it->second->y - y, 2)) >= updateRadius)
			{
				continue;
			}
			result.push_back(std::make_shared<ActorUpdate>(it->second->getUpdates(systemID)));
			result.back()->actorID = it->first;
			result.back()->actor = it->second;
			for(auto component_update = result.back()->updates.begin(); component_update != result.back()->updates.end(); component_update++)
			{
				(*component_update)->actorID = it->first;
			}
		}

	}

	if(deletedActorApproves[systemID] != deletedActors.size() - 1)
	{
		auto deleteUpdate = std::make_shared<DeleteUpdate>(deletedActors.size() - 1);
		for(int i = deletedActorApproves[systemID] + 1; i < deletedActors.size(); i++)
		{
			deleteUpdate->deletedActors.push_back(deletedActors[i]);
		}
		deleteUpdate->actorID = -1;
		auto deleteActorUpdate = std::make_shared<ActorUpdate>();
		deleteActorUpdate->actorID = -1;
		deleteActorUpdate->updates.push_back(deleteUpdate);
		result.push_back(deleteActorUpdate);
	}
	return result;
}

int GameLogic::registerSystem()
{
	systemCount++;
	deletedActorApproves[systemCount] = deletedActors.size() - 1;
	return systemCount;
}

int GameLogic::createActor(std::string actorID)
{
	
	actorCount++;
	actors[actorCount] = actorFactory.createActor(actorID);
	onEvent(Event("actor_id", false, actorCount), false);
	return actorCount;
}

void GameLogic::destroyActor(int actorID)
{
	if(actors.find(actorID) != actors.end())
	{
		bool isReplaced = false;
		for(int i = 0; i < replaceEvents.size(); i++)
		{
			if(replaceEvents[i].actorID == actorID)
			{
				isReplaced = true;
				break;
			}
		}
		if(!isReplaced)
		{
			deletedActors.push_back(actorID);
		}
		actors.erase(actors.find(actorID));
	}
}





