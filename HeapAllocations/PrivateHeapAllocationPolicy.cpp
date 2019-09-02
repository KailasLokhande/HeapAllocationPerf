#include "pch.h"
#include "PrivateHeapAllocationPolicy.h"

namespace Reliability
{
	
	const DWORD     c_defaultDwFlags = 0;
	const size_t    c_initialHeapSize = 1024 * 1024;    // Initial size    
	const size_t    c_maxHeapSize = 0;    // unbound

	PrivateHeapAllocationPolicy::PrivateHeapAllocationPolicy()
		: mHeapHandle(::HeapCreate(c_defaultDwFlags, c_initialHeapSize, c_maxHeapSize))
	{
	}

	PrivateHeapAllocationPolicy::~PrivateHeapAllocationPolicy()
	{
		if (mHeapHandle != nullptr)
		{
			(void)::HeapDestroy(mHeapHandle);
			mHeapHandle = nullptr;
		}
	}

	LPVOID PrivateHeapAllocationPolicy::allocate(std::size_t bytesToAllocate)
	{
		if (mHeapHandle == nullptr)
		{
			return nullptr;
		}

		return ::HeapAlloc(mHeapHandle, c_defaultDwFlags, bytesToAllocate);
	}

	void PrivateHeapAllocationPolicy::deallocate(LPVOID p, std::size_t num)
	{
		if ((mHeapHandle != nullptr) && (p != nullptr))
		{
			(void)::HeapFree(mHeapHandle, c_defaultDwFlags, p);
		}
	}

}
