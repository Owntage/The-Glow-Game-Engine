#ifndef WORLD_H
#define WORLD_H

#include <Box2D/Box2D.h>
#include <string>
#include <memory>
#include <utility>
#include <map>
#include <set>
#include <vector>
#include "../gui_source/singleton.h"
#include "properties_loader.h"

#define DEFAULT_FRICTION 12.0
#define WORLD_PROPERTIES "res/world_properties.xml"

class ContactData
{
public:
	ContactData()
	{
		hit = false;
		release = false;
		cancelled = false;
	}
	bool hit;
	bool release;
	bool cancelled;
	bool isSensor;
	std::string type;
	std::string otherType;
	int actorID;
	int otherActorID;
};


class MyContactListener: public b2ContactListener
{
private:
	std::set<std::pair<std::string, std::string>> pairs;
public:
	void addPair(std::pair<std::string, std::string> p);
	void clearPairs();
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

struct WorldPropertiesVisitor : XmlVisitor
{
	std::string getPropertiesFilename() const;
	void onVisit(const boost::property_tree::ptree& node, std::string nodeName) const;
};



class World: public Singleton<World>
{
private:
	std::shared_ptr<b2World> world;
	std::shared_ptr<MyContactListener> contactListener;
	float time;
	bool propertiesLoaded;
public:
	World();
	void update(double fps);
	void addPair(std::string first, std::string second);
	float getTime();
	//b2World* getWorld();
	b2Body* createStaticBody(double x, double y, double width = 0.5, double height = 0.5);
	b2Body* createDynamicBody(double x, double y);
	b2Body* createDynamicCircle(double x, double y, double r = 0.5);
	void destroyBody(b2Body* body);
	static void setFriction(b2Body* body, double friction);
	static void setDensity(b2Body* body, double density);
	static void setRestitution(b2Body* body, double restitution);
	static void setContactData(b2Body* body, ContactData& contactData);
};

#endif
