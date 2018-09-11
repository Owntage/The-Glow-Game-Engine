#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "icomponent.h"
#include "world.h"
#include <string>

struct PhysicsEvent : Event
{
	PhysicsEvent(int actorID, int otherActorID, std::string otherType, bool release) : 
		Event("physics", false, actorID),
		otherActorID(otherActorID),
		otherType(otherType),
		release(release)
	{}
	int otherActorID;
	bool release;
	std::string otherType;
};

struct AngleEvent : Event
{
	AngleEvent(int actorID, float angle) : 
		Event("angle", false, actorID),
		angle(angle) {}

	float angle;


};

struct PhysicsComponent : IComponent
{
	PhysicsComponent();
	~PhysicsComponent();
	void onEvent(const Event& event);
	void onRequest(const Request& request);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	int thisActorID;
	ContactData contactData;

	float bodyAngle;
	float mouseAngle;
	float angularSpeed;

	bool up, down, left, right;
	float speed;
	float width;
	float height;
	float density;
	float friction;
	float restitution;
	bool wasMoving;
	bool isSensor;
	b2Body* body;

};

#endif