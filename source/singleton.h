#ifndef SINGLETON_H
#define SINGLETON_H

#include <fstream>

template <typename T>
struct Singleton
{
	static T* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new T;
		}
		return instance;
	}
protected:
	Singleton() {}
	~Singleton() {}
private:
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
	static T* instance;
	
};

template <typename T>
T* Singleton<T>::instance = nullptr;






#endif