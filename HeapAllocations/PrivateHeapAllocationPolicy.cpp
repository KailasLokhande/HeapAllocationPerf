#include "pch.h"
#include "PrivateHeapAllocationPolicy.h"

namespace Reliability
{
	template<class T>
	PrivateHeapAllocationPolicy<T>::PrivateHeapAllocationPolicy()
		: mHeapHandle(::HeapCreate(0, 0, 0))
	{
	}

	template<class T>
	PrivateHeapAllocationPolicy<T>::~PrivateHeapAllocationPolicy()
	{
		if (mHeapHandle != nullptr)
		{
			(void)::HeapDestroy(mHeapHandle);
			mHeapHandle = nullptr;
		}
	}

	template<class T>
	T* PrivateHeapAllocationPolicy<T>::allocate(std::size_t bytesToAllocate)
	{
		if (mHeapHandle == nullptr)
		{
			return nullptr;
		}

		return (T*)(::HeapAlloc(mHeapHandle, 0, bytesToAllocate));
	}

	template<class T>
	void PrivateHeapAllocationPolicy<T>::deallocate(T* p, std::size_t num)
	{
		if ((mHeapHandle != nullptr) && (p != nullptr))
		{
			(void)::HeapFree(mHeapHandle, 0, p);
		}
	}

	template class PrivateHeapAllocationPolicy<PHANDLE>;
}
