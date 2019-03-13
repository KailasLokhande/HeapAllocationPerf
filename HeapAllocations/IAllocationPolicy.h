#pragma once

#include <cstddef>
#include <Windows.h>
#include <xutility>
namespace Reliability
{
	class IAllocationPolicy
	{
	public:
		virtual LPVOID allocate(std::size_t numberOfBytes) = 0;
		virtual void deallocate(LPVOID p, std::size_t numberOfBytes) = 0;
		virtual ~IAllocationPolicy() = default;
		IAllocationPolicy() = default;
		IAllocationPolicy(IAllocationPolicy&&) = default;
	};

	//template class IAllocationPolicy<PHANDLE>;
	//template class IAllocationPolicy<std::_Container_proxy>;
}
