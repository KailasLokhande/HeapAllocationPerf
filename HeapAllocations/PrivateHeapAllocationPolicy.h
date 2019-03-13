#pragma once

#include "IAllocationPolicy.h"
#include <Windows.h>

namespace Reliability
{
	template<class T>
	class PrivateHeapAllocationPolicy : public IAllocationPolicy<T>
	{
	public:
		PrivateHeapAllocationPolicy();
		virtual ~PrivateHeapAllocationPolicy();
		virtual T* allocate(std::size_t num);
		virtual void deallocate(T* p, std::size_t num);
	
	private:
		HANDLE mHeapHandle;
	};
}
