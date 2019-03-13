#pragma once
#include <cstddef>
#include "IAllocationPolicy.h"
#include <mutex>
namespace Reliability
{
	std::mutex mut;
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
		ReplicationAllocator(std::shared_ptr<IAllocationPolicy>& allocationPolicy) : mAllocationPolicy(allocationPolicy) {}
		ReplicationAllocator(const ReplicationAllocator<T>& other) : mAllocationPolicy(other.mAllocationPolicy) {}

		template <class U> struct rebind {
			using other = ReplicationAllocator<U>;
		};

		template <class U>
		ReplicationAllocator(const ReplicationAllocator<U>& other) : mAllocationPolicy(other.mAllocationPolicy) {}

		// Destructor
		~ReplicationAllocator() throw() {}

		// allocates memory for num elements.
		pointer allocate(size_type num)
		{
			/*{
				std::lock_guard<std::mutex> lock(mut);
				std::cerr << "allocate " << num << " element(s)"
					<< " of size " << sizeof(T) << std::endl;
			}*/
			pointer ret = static_cast<pointer>(mAllocationPolicy->allocate(num * sizeof(T)));
			/*{
				std::lock_guard<std::mutex> lock(mut);
				std::cerr << " allocated at: " << (void*)ret << std::endl;
			}*/
			return ret;
		}

		// Deallocates memory for num elements to which p refers.
		void deallocate(pointer p, size_type num = 0)
		{
			/*{
				std::lock_guard<std::mutex> lock(mut);
				std::cerr << "deallocate " << num << " element(s)"
					<< " of size " << sizeof(T)
					<< " at: " << (void*)p << std::endl;
			}*/
			mAllocationPolicy->deallocate(p, num);
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
		std::shared_ptr<IAllocationPolicy> mAllocationPolicy;
	};
}

