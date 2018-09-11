#include "world.h"


std::string WorldPropertiesVisitor::getPropertiesFilename() const
{
	return WORLD_PROPERTIES;
}

void WorldPropertiesVisitor::onVisit(const boost::property_tree::ptree& node, std::string nodeName) const
{
	if(nodeName == "cancel_collision")
	{
		World::getInstance()->addPair(node.get<std::string>("first"), node.get<std::string>("second"));
	}
}


World::World()
{
	b2Vec2 gravity(0,0);
	//world = new b2World(gravity);
	world = std::make_shared<b2World>(gravity);
	contactListener = std::make_shared<MyContactListener>();
	world->SetContactListener(&*contactListener);
	time = 0;
	propertiesLoaded = false;
	//WorldPropertiesVisitor visitor;
	//walkXml(visitor.getPropertiesFilename(), visitor);
}

void World::addPair(std::string first, std::string second)
{
	contactListener->addPair(std::make_pair(first, second));
}

void World::update(double fps)
{
	if(!propertiesLoaded)
	{
		WorldPropertiesVisitor visitor;
		walkXml(visitor.getPropertiesFilename(), visitor);
		propertiesLoaded = true;
	}
	world->Step(1.0/fps,6,2);
	time += 1.0 / fps;
}

float World::getTime()
{
	return time;
}

/*
b2World* World::getWorld()
{
	return world;
}
*/

b2Body* World::createStaticBody(double x, double y, double width, double height)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_staticBody;
	result = world->CreateBody(&bodyDef);
	b2PolygonShape shapeDef;
	shapeDef.SetAsBox(width / 2.0, height / 2.0);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

b2Body* World::createDynamicBody(double x, double y)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = false;
	result = world->CreateBody(&bodyDef);
	b2PolygonShape shapeDef;
	shapeDef.SetAsBox(0.5, 0.5);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

b2Body* World::createDynamicCircle(double x, double y, double r)
{
	b2Body* result;
	b2BodyDef bodyDef;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0.0;
	bodyDef.type = b2_dynamicBody;
	result = world->CreateBody(&bodyDef);
	b2CircleShape shapeDef;
	shapeDef.m_radius = r;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	result->CreateFixture(&fixtureDef);
	return result;
}

void World::destroyBody(b2Body* body)
{
	world->DestroyBody(body);
}

void World::setFriction(b2Body* body, double friction)
{
	body->GetFixtureList()->SetFriction(friction);
	body->SetLinearDamping(friction*DEFAULT_FRICTION);
}
void World::setDensity(b2Body* body, double density)
{
	body->GetFixtureList()->SetDensity(density);
}
void World::setRestitution(b2Body* body, double restitution)
{
	body->GetFixtureList()->SetRestitution(restitution);
}
void World::setContactData(b2Body* body, ContactData& contactData)
{
	body->GetFixtureList()->SetUserData(&contactData);
}
