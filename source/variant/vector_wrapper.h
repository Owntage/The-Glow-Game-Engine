#ifndef NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H
#define NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H

template<typename Type>
struct VectorWrapper
{
	std::vector<Type> vec;

	VectorWrapper(std::vector<Type> vec) : vec(vec)
	{}

	VectorWrapper() {}

	template<typename StreamType>
	friend StreamType& operator<<(StreamType& s, VectorWrapper<Type>& v)
	{
		s << (int) v.vec.size();
		for (int i = 0; i < v.vec.size(); i++)
		{
			s << v.vec[i];
		}
		return s;
	}

	template<typename StreamType>
	friend StreamType& operator>>(StreamType& s, VectorWrapper<Type>& v)
	{
		int size;
		s >> size;
		v.vec.resize(size);
		for (int i = 0; i < size; i++)
		{
			s >> v.vec[i];
		}
		return s;
	}
};

#endif //NETWORK_SHOOTER_PROJECT_VECTOR_WRAPPER_H
