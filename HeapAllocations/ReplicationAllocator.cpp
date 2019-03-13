#include "pch.h"
#include "ReplicationAllocator.h"

namespace Reliability
{
	template<class T>
	ReplicationAllocator<T>::ReplicationAllocator() throw()
	{
	}

	template<class T>
	ReplicationAllocator<T>::ReplicationAllocator(const ReplicationAllocator<T>& other)
	{

	}

	template<class T>
	template<class U>
	ReplicationAllocator<T>::ReplicationAllocator(const ReplicationAllocator<U>& other) throw()
	{

	}

	template<class T>
	ReplicationAllocator<T>::~ReplicationAllocator() throw()
	{

	}

	template<class T>
	auto ReplicationAllocator<T>::allocate(size_type num) -> pointer
	{
		std::cerr << "allocate " << num << " element(s)"
			<< " of size " << sizeof(T) << std::endl;
		pointer ret = static_cast<pointer>(::operator new(num * sizeof(T)));
		std::cerr << " allocated at: " << (void*)ret << std::endl;
		return ret;
	}

	template<class T>
	void ReplicationAllocator<T>::deallocate(pointer p, size_type num)
	{
		std::cerr << "deallocate " << num << " element(s)"
			<< " of size " << sizeof(T)
			<< " at: " << (void*)p << std::endl;
		::operator delete(p);
	}
}
