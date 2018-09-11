#include "physics_component.h"
#include "move_event.h"
#include "coord_event.h"
#include "variant_update.h"
#include <math.h>


PhysicsComponent::PhysicsComponent() :
	up(false),
	down(false),
	left(false),
	right(false),
	wasMoving(false),
	speed(3.0f),
	bodyAngle(0.0f),
	mouseAngle(0.0f)
{
	body = nullptr;
}

PhysicsComponent::~PhysicsComponent()
{
	if(body != nullptr)
	{
		World::getInstance()->destroyBody(body);
	}
	
}

void PhysicsComponent::onRequest(const Request& request)
{
	if(request.name == "coords")
	{
		request.callback(CoordEvent("", 0, body->GetPosition().x, body->GetPosition().y));
	}
	if(request.name == "angle")
	{
		request.callback(AngleEvent(0, bodyAngle));
	}
}

static const float PI = 3.141593;

static float getAngle(float x, float y)
{
	
	if(x < 0)
	{
		return atan(y / x) + PI;
	}
	else
	{
		if(y < 0)
		{
			return atan(y / x) + 2 * PI;
		}
		else
		{
			return atan(y / x);
		}
	}
}

//contract: 0 <= destAngle, currentAngle <= 2 * PI
//return value: how much should add to currentAngle to become destAngle
static float getRelativeAngle(float destAngle, float currentAngle)
{
	float deltaAngle = destAngle - currentAngle;
	if(deltaAngle < 0)
	{
		deltaAngle += PI * 2;
	}
	//std::cout << "delta angle: " << deltaAngle;
	return deltaAngle;
}

static float myAbs(float a)
{
	if(a < 0) {
		return -a;
	}
	return a;
}

void PhysicsComponent::onEvent(const Event& event)
{
	if(event.name == "actor_id")
	{
		thisActorID = event.actorID;
		contactData.actorID = thisActorID;
	}
	if(event.name == "move")
	{
		const MoveEvent& moveEvent = (const MoveEvent&) event;
		up = moveEvent.up;
		down = moveEvent.down;
		left = moveEvent.left;
		right = moveEvent.right;
		mouseAngle = moveEvent.angle / 180.0f * PI;
	}
	if(event.name == "timer")
	{
		for(int i = 0; i < 5; i++)
		{
			if(myAbs(mouseAngle - bodyAngle) > angularSpeed / 300.0f)
			{
				int direction = 1;
				
				if(getRelativeAngle(mouseAngle, bodyAngle) > PI)
				{
					direction = -1;
				}
				bodyAngle += direction * angularSpeed / 300.0f;
				if(direction == 1 && bodyAngle > 2 * PI)
				{
					bodyAngle -= 2 * PI;
				}
				else if(direction == -1 && bodyAngle < 0)
				{
					bodyAngle += 2 * PI;
				}
			}
		}


		b2Vec2 direction(0, 0);
		if(right) direction += b2Vec2(1, 0);
		if(left) direction += b2Vec2(-1, 0);
		if(up) direction += b2Vec2(0, -1);
		if(down) direction += b2Vec2(0, 1);
		if(body->GetLinearVelocity().LengthSquared() > 0)
		{
			currentDataNumber++;
			wasMoving = true;
		}
		else
		{
			if(wasMoving)
			{
				currentDataNumber++;
				wasMoving = false;
			}
		}

		direction.Normalize();
		if(body->GetLinearVelocity().LengthSquared() < speed * speed)
		{
			body->ApplyForceToCenter(60.0 * direction, true);
		}
		
		if(contactData.hit && !contactData.cancelled)
		{
			contactData.hit = false;
			localEvents.push_back(std::make_shared<PhysicsEvent>(thisActorID, 
				contactData.otherActorID, 
				contactData.otherType,
				contactData.release));
		}

	}
	if(event.name == "set_coords")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		body->SetTransform(b2Vec2(coordEvent.x, coordEvent.y), body->GetAngle());
		currentDataNumber++;
	}
	if(event.name == "set_speed")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		body->SetLinearVelocity(b2Vec2(coordEvent.x, coordEvent.y));
	}
	if(event.name == "set_scale")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		b2Body* newBody;
		if(body->GetType() != b2_dynamicBody)
		{
			currentDataNumber++;
			width = coordEvent.x;
			height = coordEvent.y;
			newBody = World::getInstance()->createStaticBody(body->GetPosition().x, body->GetPosition().y, coordEvent.x, coordEvent.y);
			World::getInstance()->destroyBody(body);
			body = newBody;
			World::getInstance()->setFriction(body, friction);
			World::getInstance()->setDensity(body, density);
			World::getInstance()->setRestitution(body, restitution);
			World::getInstance()->setContactData(body, contactData);
		}
		//deal with dynamic bodies...
		
	}
}

bool PhysicsComponent::hasUpdate(int systemID)
{
	if(lastSystemApproved.find(systemID) == lastSystemApproved.end())
	{
		return true;
	}
	else
	{
		return lastSystemApproved[systemID] < currentDataNumber;
	}
}

std::string PhysicsComponent::getName()
{
	return "move";
}

std::shared_ptr<ComponentUpdate> PhysicsComponent::getUpdate(int systemID)
{
	auto result = std::make_shared<VariantUpdate>("move");
	result->set("x", body->GetPosition().x);
	result->set("y", body->GetPosition().y);
	result->set("sizeX", width);
	result->set("sizeY", height);
	result->set("time", World::getInstance()->getTime());
	result->set("speedX", body->GetLinearVelocity().x);
	result->set("speedY", body->GetLinearVelocity().y);
	result->number = currentDataNumber;
	return result;
}

std::shared_ptr<IComponent> PhysicsComponent::loadFromXml(const boost::property_tree::ptree& tree)
{

	std::shared_ptr<PhysicsComponent> result = std::make_shared<PhysicsComponent>();
	double x = tree.get("x", 0.0);
	double y = tree.get("y", 0.0);
	result->speed = tree.get("speed", 0.0);
	std::string type = tree.get<std::string>("type");
	std::string id = tree.get<std::string>("id");
	if(type == "static")
	{
		double width = tree.get("width", 0.5);
		double height = tree.get("height", 0.5);
		result->width = width;
		result->height = height;
		result->body = World::getInstance()->createStaticBody(x, y, width, height);
	}
	if(type == "dynamic")
	{
		result->body = World::getInstance()->createDynamicBody(x, y);
	}
	if(type == "circle")
	{
		double radius = tree.get("radius", 0.5);
		result->body = World::getInstance()->createDynamicCircle(x, y, radius);
	}
	double density = tree.get("density", 1.0);
	double restitution = tree.get("restitution", 0.2);
	double friction = tree.get("friction", 0.5);
	bool isSensor = tree.get("is_sensor", "false") == "true";
	result->contactData.isSensor = isSensor;
	
	result->angularSpeed = tree.get("angular_speed", 3.14f);

	result->density = density;
	result->restitution = restitution;
	result->friction = friction;
	World::setDensity(result->body, density);
	World::setRestitution(result->body, restitution);
	World::setFriction(result->body, friction);
	result->contactData.type = id;
	World::setContactData(result->body, result->contactData);
	return result;
}

