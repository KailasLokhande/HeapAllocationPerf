#pragma once
#include <cstddef>
#include "IAllocationPolicy.h"

namespace Reliability
{
	template <class T>
	class ReplicationAllocator {
	
	public:
		// type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		// constructors
		//ReplicationAllocator();
		//ReplicationAllocator() noexcept : mAllocationPolicy(nullptr) {}
		ReplicationAllocator(IAllocationPolicy<T>* pAllocationPolicy) : mAllocationPolicy(pAllocationPolicy) {}
		ReplicationAllocator(const ReplicationAllocator<T>& other) : mAllocationPolicy(other.mAllocationPolicy) {}

		template <class U> struct rebind {
			using other = ReplicationAllocator<U>;
		};

		template <class U>
		ReplicationAllocator(const ReplicationAllocator<U>& other) : mAllocationPolicy{ const_cast< IAllocationPolicy<T>*>( other.mAllocationPolicy )}
		{
			/*mAllocationPolicy = const_cast< std::unique_ptr<IAllocationPolicy<U>>>(other.mAllocationPolicy);*/
			//if ()
			std::cerr << "CHECK WHY" << std::endl;
		}

		//template <>
		//ReplicationAllocator(const ReplicationAllocator<std::nullptr_t>& other) noexcept : mAllocationPolicy(nullptr)
		//{
		//	/*mAllocationPolicy = const_cast< std::unique_ptr<IAllocationPolicy<U>>>(other.mAllocationPolicy);*/
		//	//if ()
		//	std::cerr << "CHECK WHY" << std::endl;
		//}

		//template <>
		//ReplicationAllocator(const ReplicationAllocator<std::nullptr_t>& other) noexcept : mAllocationPolicy(nullptr)
		//{
		//	/*mAllocationPolicy = const_cast< std::unique_ptr<IAllocationPolicy<U>>>(other.mAllocationPolicy);*/
		//	//if ()
		//	std::cerr << "CHECK WHY" << std::endl;
		//}


		// Destructor
		~ReplicationAllocator() throw() {}

		// allocates memory for num elements.
		pointer allocate(size_type num)
		{
			std::cerr << "allocate " << num << " element(s)"
				<< " of size " << sizeof(T) << std::endl;
			pointer ret = mAllocationPolicy->allocate(num * sizeof(T));
		/*	if (mAllocationPolicy != nullptr)
			{
				ret = static_cast<pointer>(mAllocationPolicy->allocate(num * sizeof(T)));
			}
			else
			{
			    ret = static_cast<pointer>(::operator new(num * sizeof(T)));
			}*/
			std::cerr << " allocated at: " << (void*)ret << std::endl;
			return ret;
		}

		// Deallocates memory for num elements to which p refers.
		void deallocate(pointer p, size_type num)
		{
			std::cerr << "deallocate " << num << " element(s)"
				<< " of size " << sizeof(T)
				<< " at: " << (void*)p << std::endl;
			::operator delete(p);
		}

		template <class U>
		auto operator==(const ReplicationAllocator<U>& other) const noexcept
		{
			// Allocator as same only if allocation policies are same.
			return mAllocationPolicy == other.mAllocationPolicy;
		}

		template <class U>
		auto operator!= (const ReplicationAllocator<U>& other) const noexcept
		{
			return !(*this == other);
		}

		template <class U> friend class ReplicationAllocator;
	private:
		IAllocationPolicy<T>* mAllocationPolicy;
	};
}

