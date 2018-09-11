#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "../event.h"
#include "../request.h"
#include "component_update.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <boost/property_tree/ptree.hpp>

struct IComponent
{
	IComponent() : currentDataNumber(1) {}
	virtual ~IComponent() {}
	virtual void onEvent(const Event& event) = 0;
	virtual void onRequest(const Request& request) {}
	virtual bool hasUpdate(int systemID) = 0;
	void approve(int systemID, int number)
	{
		lastSystemApproved[systemID] = number;
	}
	virtual std::string getName() = 0;
	virtual std::shared_ptr<ComponentUpdate>  getUpdate(int syatemID) = 0;
	virtual std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree) = 0;
	std::vector<std::shared_ptr<Event> > globalEvents;
	std::vector<std::shared_ptr<Event> > localEvents;
	std::vector<std::shared_ptr<Request> > requests;
	void setCurrentNumber(int currentDataNumber)
	{
		this->currentDataNumber = currentDataNumber;
	}
	int getCurrentNumber()
	{
		return currentDataNumber; 
	}
protected:
	std::map<int, int> lastSystemApproved;
	//std::map<int, int> currentSystemNumber;
	int currentDataNumber;
};

#endif