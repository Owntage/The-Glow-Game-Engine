#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include "actor_factory.h"
#include "actor_update.h"
#include "event.h"

#define UPDATE_RADIUS 10

struct RecreateData
{
	RecreateData(std::string type, int actorID, int dataNumber) :
		type(type),
		actorID(actorID),
		dataNumber(dataNumber)
	{}
	std::string type;
	int actorID;
	int dataNumber;
};

struct GameLogic
{
	GameLogic(ActorFactory& actorFactory): actorFactory(actorFactory), systemCount(0), actorCount(0) {}
	void onEvent(const Event& event);
	
	void approve(int actorID, std::string component, int systemID, int number);
	std::vector<std::shared_ptr<ActorUpdate> > getUpdates(int systemID);
	std::vector<std::shared_ptr<ActorUpdate> > getUpdates(int systemID, float x, float y, float updateRadius);
	int registerSystem();
	int createActor(std::string id);
	void destroyActor(int number);
private:
	void thrownEventHandler(std::vector<std::shared_ptr<Event> >& events, bool global, int actorID);
	void onEvent(const Event& event, bool shouldDelete);
	std::set<int> actorsMarkedToDelete;
	std::vector<std::shared_ptr<Event> > createEvents;
	std::vector<RecreateData> replaceEvents;

	ActorFactory& actorFactory;
	int systemCount;
	int actorCount;
	std::map<int, std::shared_ptr<Actor> > actors;
	std::map<int, int> deletedActorApproves;
	std::vector<int> deletedActors;
};

#endif