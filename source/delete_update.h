#ifndef DELETE_UPDATE
#define DELETE_UPDATE

#include "components/component_update.h"
#include <vector>

struct DeleteUpdate : ComponentUpdate
{
	DeleteUpdate(int number) : ComponentUpdate("delete")
	{
		this->number = number;
	}
	DeleteUpdate() : ComponentUpdate("delete") {}
	std::vector<int> deletedActors;
	
	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, DeleteUpdate& u)
	{
		s << (int) u.deletedActors.size();
		for(int i = 0; i < u.deletedActors.size(); i++)
		{
			s << u.deletedActors[i];
		}
		return s;
	}
	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, DeleteUpdate& u)
	{
		int size;
		s >> size;
		for(int i = 0; i < size; i++)
		{
			int temp;
			s >> temp;
			u.deletedActors.push_back(temp);
		}
		return s;
	}

};

#endif