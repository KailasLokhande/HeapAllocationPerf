#pragma once

#include "IAllocationPolicy.h"
#include <Windows.h>

namespace Reliability
{
	class PrivateHeapAllocationPolicy : public IAllocationPolicy
	{
	public:
		PrivateHeapAllocationPolicy();
		virtual ~PrivateHeapAllocationPolicy();
		virtual LPVOID allocate(std::size_t num) override;
		virtual void deallocate(LPVOID p, std::size_t num) override;
	
	private:
		HANDLE mHeapHandle;
	};
}
