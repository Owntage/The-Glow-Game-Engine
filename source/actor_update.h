#ifndef ACTOR_UPDATE_H
#define ACTOR_UPDATE_H

#include <vector>
#include <memory>
#include "components/component_update.h"

struct Actor;

struct ActorUpdate
{
	int actorID;
	std::shared_ptr<Actor> actor;
	std::vector<std::shared_ptr<ComponentUpdate>> updates;
};

#endif