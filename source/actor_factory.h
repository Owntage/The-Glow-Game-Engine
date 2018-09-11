#ifndef ACTOR_FACTORY_H
#define ACTOR_FACTORY_H

#include "actor.h"
#include <memory>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <map>

struct ActorFactory
{
	ActorFactory(std::string propertiesPath);
	std::shared_ptr<Actor> createActor(std::string id);
private:
	ActorFactory(const ActorFactory& other) {}
	ActorFactory& operator=(const ActorFactory& other) {}
	std::map<std::string, boost::property_tree::ptree> actors;
};

#endif