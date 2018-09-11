#ifndef NETWORK_SHOOTER_PROJECT_VARIANTUPDATE_H
#define NETWORK_SHOOTER_PROJECT_VARIANTUPDATE_H

#include "../variant/variant_map.h"

struct VariantUpdate : ComponentUpdate, VariantMap
{
	VariantUpdate(std::string name) :
		ComponentUpdate("variant")
	{
		set("name", name);
	}

	VariantUpdate() {}

	template<typename StreamType>
	friend StreamType& operator<<(StreamType& s, VariantUpdate& u)
	{
		s << (ComponentUpdate&) u << (VariantMap&) u;
		return s;
	}

	template<typename StreamType>
	friend StreamType& operator>>(StreamType& s, VariantUpdate& u)
	{
		s >> (ComponentUpdate&) u >> (VariantMap&) u;
		return s;
	}
};

#endif //NETWORK_SHOOTER_PROJECT_VARIANTUPDATE_H
