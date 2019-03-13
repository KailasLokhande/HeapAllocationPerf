#pragma once

#include <cstddef>
#include <Windows.h>
#include <xutility>
namespace Reliability
{
	template<class T>
	class IAllocationPolicy
	{
	public:
		virtual T* allocate(std::size_t num) { return static_cast<T*>(::operator new(num * sizeof(T))); };
		virtual void deallocate(T* p, std::size_t num) { ::operator delete(p); };
		virtual ~IAllocationPolicy() = default;
		IAllocationPolicy() {};
		template <class U>
		inline IAllocationPolicy(const IAllocationPolicy<U>& other) {}
	};

	//template class IAllocationPolicy<PHANDLE>;
	//template class IAllocationPolicy<std::_Container_proxy>;
}
