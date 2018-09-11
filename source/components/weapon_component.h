#ifndef WEAPON_COMPONENT_H
#define WEAPON_COMPONENT_H

#include "icomponent.h"
#include "component_update.h"
#include "../event.h"
#include "../properties_loader.h"
#include <singleton.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <vector>

#define WEAPON_PROPERTIES_FILE "res/weapon_properties.xml"

struct WeaponPropertiesVisitor : XmlVisitor
{
	std::string getPropertiesFilename() const;
	void onVisit(const boost::property_tree::ptree& tree, std::string nodeName) const;
};


struct WeaponDef //NB: not everything is serialized
{
	float period;
	float soundPeriod;
	float bulletSpeed;
	float reloadTime;
	float dispersion; //in radians
	float damage;
	int holders;
	int bullets;
	int bulletsPerHolder;
	std::string bulletType;
	std::string weaponTexture;
	std::string soundName;
	std::string layerName;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, WeaponDef& w)
	{
		s << w.period << w.bulletSpeed << w.reloadTime << w.dispersion << w.holders << w.bulletsPerHolder
			<< w.bulletType << w.weaponTexture;
		return s;
	}
	
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, WeaponDef& w)
	{
		s >> w.period >> w.bulletSpeed >> w.reloadTime >> w.dispersion >> w.holders >> w.bulletsPerHolder
			>> w.bulletType >> w.weaponTexture;
		return s;
	}
	
};

struct WeaponData
{
	WeaponData() :
		timeSinceShot(9999.0f),
		timeSinceReload(9999.0f),
		shotsMade(0),
		holdersSpent(0)
	{}

	float timeSinceShot;
	float timeSinceReload;
	int shotsMade;
	int holdersSpent;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, WeaponData& w)
	{
		s << w.timeSinceShot << w.timeSinceReload << w.shotsMade << w.holdersSpent;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, WeaponData& w)
	{
		s >> w.timeSinceShot >> w.timeSinceReload >> w.shotsMade >> w.holdersSpent;
		return s;
	}
};


struct WeaponEvent : Event
{
	WeaponEvent(int actorID, std::string weaponID) : 
		Event("weapon", false, actorID),
		weaponID(weaponID)
	{}
	std::string weaponID;
};

struct WeaponUpdate : ComponentUpdate
{
	enum class WeaponState;
	WeaponUpdate(WeaponDef& weaponDef, WeaponData& weaponData, WeaponState state) :
		ComponentUpdate("weapon"),
		weaponDef(weaponDef),
		state(state),
		weaponData(weaponData)
	{}
	WeaponUpdate() :
		ComponentUpdate("weapon")
	{}
	enum class WeaponState
	{
		SHOOT,
		RELOAD,
		CHANGE
	};
	WeaponDef weaponDef;
	WeaponData weaponData;
	WeaponState state;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, WeaponUpdate& u)
	{
		s << u.weaponDef << u.weaponData << (int) u.state;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, WeaponUpdate& u)
	{
		s >> u.weaponDef >> u.weaponData >> (int&) u.state;
		return s;
	}
};



struct WeaponComponent : IComponent
{
	WeaponComponent();
	void onEvent(const Event& event);
	bool hasUpdate(int systemID);
	std::string getName();
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	void shoot();
	void pushBulletCreateEvent(const Event& event, float angle);
	void pushSoundEvent();
	friend class WeaponPropertiesVisitor;
	static std::map<std::string, WeaponDef> weaponDefinitions;
	std::vector<WeaponDef> weapons;
	int thisActorID;
	float timeSinceSoundPlayed;
	int currentWeapon;
	bool isShooting;
	WeaponUpdate::WeaponState state;
	
	std::vector<WeaponData> weaponData;
};

#endif