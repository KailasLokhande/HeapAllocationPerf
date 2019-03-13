#include "pch.h"
#include "PrivateHeapAllocationPolicy.h"

namespace Reliability
{
	PrivateHeapAllocationPolicy::PrivateHeapAllocationPolicy()
		: mHeapHandle(::HeapCreate(0, 0, 0))
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

		return ::HeapAlloc(mHeapHandle, 0, bytesToAllocate);
	}

	void PrivateHeapAllocationPolicy::deallocate(LPVOID p, std::size_t num)
	{
		if ((mHeapHandle != nullptr) && (p != nullptr))
		{
			(void)::HeapFree(mHeapHandle, 0, p);
		}
	}

}
