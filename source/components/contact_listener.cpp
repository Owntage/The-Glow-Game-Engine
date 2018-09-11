#include "world.h"

void MyContactListener::addPair(std::pair<std::string, std::string> p)
{
	pairs.insert(p);
	std::pair<std::string, std::string> mirrored;
	mirrored.first = p.second;
	mirrored.second = p.first;
	pairs.insert(mirrored);
}

void MyContactListener::clearPairs()
{
	pairs.clear();
}

void MyContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	
	if(fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		((ContactData*)(fixtureA->GetUserData()))->hit = true;
		((ContactData*)(fixtureA->GetUserData()))->release = false;
		((ContactData*)(fixtureA->GetUserData()))->otherType = ((ContactData*)(fixtureB->GetUserData()))->type;
		((ContactData*)(fixtureA->GetUserData()))->otherActorID = ((ContactData*)(fixtureB->GetUserData()))->actorID;
		

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = false;
		((ContactData*)(fixtureB->GetUserData()))->otherActorID = ((ContactData*)(fixtureA->GetUserData()))->actorID;
		
		
	}
}

void MyContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if(fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		((ContactData*)(fixtureA->GetUserData()))->hit = true;
		((ContactData*)(fixtureA->GetUserData()))->release = true;
		((ContactData*)(fixtureA->GetUserData()))->otherType = ((ContactData*)(fixtureB->GetUserData()))->type;
		((ContactData*)(fixtureA->GetUserData()))->otherActorID = ((ContactData*)(fixtureB->GetUserData()))->actorID;

		((ContactData*)(fixtureB->GetUserData()))->hit = true;
		((ContactData*)(fixtureB->GetUserData()))->release = true;
		((ContactData*)(fixtureB->GetUserData()))->otherActorID = ((ContactData*)(fixtureA->GetUserData()))->actorID;
	}
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) 
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	if(fixtureA->GetUserData() && fixtureB->GetUserData())
	{
		ContactData* firstBody = (ContactData*) fixtureA->GetUserData();
		ContactData* secondBody = (ContactData*) fixtureB->GetUserData();
		auto p = std::make_pair(firstBody->type, secondBody->type);
		if(pairs.find(p) != pairs.end())
		{
			contact->SetEnabled(false);
			firstBody->cancelled = true;
			secondBody->cancelled = true;
		}
		else
		{
			firstBody->cancelled = false;
			secondBody->cancelled = false;
		}
		if(firstBody->isSensor || secondBody->isSensor)
		{
			contact->SetEnabled(false);
		}
	}
}

void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}