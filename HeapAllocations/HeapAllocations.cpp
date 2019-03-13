// HeapAllocations.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <intsafe.h>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include "Arguments.h"
#undef max
#include <limits>
#include <map>
#include "ReplicationAllocator.h"
#include "PrivateHeapAllocationPolicy.h"

const DWORD     c_defaultDwFlags = 0;
const size_t    c_initialHeapSize = 1024 * 1024;    // Initial size    
const size_t    c_maxHeapSize = 0;    // unbound
//
//class HeapRegistry
//{
//private:
//	static std::map<std::thread::id, HANDLE> threadHeap;
//
//public:
//	static HANDLE GetOrAddHeap(std::thread::id threadId)
//	{
//		if (threadHeap.find(threadId) == threadHeap.end())
//		{
//			HANDLE heapHandle = HeapCreate(c_defaultDwFlags, c_initialHeapSize, c_maxHeapSize);
//			threadHeap[threadId] = heapHandle; 
//		}
//		return threadHeap[threadId];
//	}
//
//};

	template <class T>
	class MyAlloc {
	public:
		// type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;
		HANDLE m_heapHandle;
		// rebind allocator to type U
		template <class U>
		struct rebind {
			typedef MyAlloc<U> other;
		};

		// return address of values
		pointer address(reference value) const {
			return &value;
		}
		const_pointer address(const_reference value) const {
			return &value;
		}

		/* constructors and destructor
		 * - nothing to do because the allocator has no state
		 */
		MyAlloc(HANDLE heapHandle) throw() {
			m_heapHandle = heapHandle;
		}
		MyAlloc(const MyAlloc& other) throw() {
			m_heapHandle = other.m_heapHandle;
		}
		template <class U>
		MyAlloc(const MyAlloc<U>& other) throw() {
			m_heapHandle = other.m_heapHandle;
		}
		~MyAlloc() throw() {
		}

		// return maximum number of elements that can be allocated
		size_type max_size() const throw() {
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		// allocate but don't initialize num elements of type T
		pointer allocate(size_type num, const void* = 0) {
			// print message and allocate memory with global new
			std::cerr << "allocate " << num << " element(s)"
				<< " of size " << sizeof(T) << std::endl;
			pointer ret = (pointer)(HeapAlloc(m_heapHandle, 0, num * sizeof(T)));
			std::cerr << " allocated at: " << (void*)ret << std::endl;
			return ret;
		}

		// initialize elements of allocated storage p with value value
		void construct(pointer p, const T& value) {
			// initialize memory with placement new
			new((void*)p)T(value);
		}

		// destroy elements of initialized storage p
		void destroy(pointer p) {
			// destroy objects by calling their destructor
			p->~T();
		}

		// deallocate storage p of deleted elements
		void deallocate(pointer p, size_type num) {
			// print message and deallocate memory with global delete
			std::cerr << "deallocate " << num << " element(s)"
				<< " of size " << sizeof(T)
				<< " at: " << (void*)p << std::endl;
			HeapFree(m_heapHandle, 0, p);
			//::operator delete((void*)p);
		}
	};

	template <class T1, class T2>
	bool operator== (const MyAlloc<T1>&,
		const MyAlloc<T2>&) throw() {
		return true;
	}
	template <class T1, class T2>
	bool operator!= (const MyAlloc<T1>&,
		const MyAlloc<T2>&) throw() {
		return false;
	}

class HeapAllocationRunner
{
private:
	HANDLE hDefaultProcessHeap;
	std::mutex mut;
	std::vector<std::chrono::microseconds> mAllocationTimings;
	std::vector<std::chrono::microseconds> mFreeTimings;
	Arguments& mArgs;

	void addAllocationTime(std::chrono::microseconds& microseconds)
	{
		std::lock_guard<std::mutex> lk(mut);
		mAllocationTimings.push_back(microseconds);
	}

	void addFreeTime(std::chrono::microseconds& microseconds)
	{
		std::lock_guard<std::mutex> lk(mut);
		mFreeTimings.push_back(microseconds);
	}
	
	void printResults(std::vector<std::chrono::microseconds>& list)
	{
		std::sort(list.begin(), list.end());

		auto count = list.size() - 1;
		std::cout << "Total " << count + 1 << std::endl;
		std::cout << "Min " << list[0].count() << std::endl;
		std::cout << "Max " << list[count-1].count() << std::endl;
		std::cout << "P50 " <<  list[(int)(count * 0.500)].count() << " us" << std::endl;
		std::cout << "P60 " <<  list[(int)(count * 0.600)].count() << " us" << std::endl;
		std::cout << "P70 " <<  list[(int)(count * 0.700)].count() << " us" << std::endl;
		std::cout << "P80 " <<  list[(int)(count * 0.800)].count() << " us" << std::endl;
		std::cout << "P90 " <<  list[(int)(count * 0.900)].count() << " us" << std::endl;
		std::cout << "P95 " <<  list[(int)(count * 0.950)].count() << " us" << std::endl;
		std::cout << "P99 " <<  list[(int)(count * 0.990)].count() << " us" << std::endl;
		std::cout << "P995 " << list[(int)(count * 0.995)].count() << " us" << std::endl;
		std::cout << "P999 " << list[(int)(count * 0.999)].count() << " us" << std::endl;
	}

	void printResults()
	{
		std::cout << "Allocation Timings" << std::endl;
		printResults(mAllocationTimings);

		std::cout << "Deallocation Timings" << std::endl;
		printResults(mFreeTimings);
	}

public:
	class Temp
	{
		long x;
		int y;
		double z;
	};
	void DoRun()
	{
		SIZE_T bytesToAllocate = 64 * 1024;
		const HANDLE heapHandle = (!mArgs.UsePrivateHeap) ? hDefaultProcessHeap :
			HeapCreate(c_defaultDwFlags, c_initialHeapSize, c_maxHeapSize);
		//MyAlloc<PHANDLE> myAlloc(heapHandle);
		Reliability::PrivateHeapAllocationPolicy pH;
		Reliability::ReplicationAllocator<PHANDLE> myAlloc(pH); //(std::move(std::make_unique<Reliability::PrivateHeapAllocationPolicy<PHANDLE>>()));
		for (int j = 0; j < mArgs.BatchIteration; j++)
		{
			const std::thread::id threadId = std::this_thread::get_id();
			
			std::vector<Temp*, Reliability::ReplicationAllocator<PHANDLE>> handles(myAlloc);
			handles.reserve(mArgs.BatchSize);
			for (int i = 0; i < mArgs.BatchSize; i++)
			{
				auto startTime = std::chrono::high_resolution_clock::now();
				void* heap = (void*)HeapAlloc(heapHandle, 0, bytesToAllocate);
				Temp* temp = new(heap) Temp();
				auto endTime = std::chrono::high_resolution_clock::now();
				auto executionTimeInMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
				addAllocationTime(executionTimeInMs);
				if (temp == NULL) {
					_tprintf(TEXT("HeapAlloc failed to allocate %d bytes.\n"),
						bytesToAllocate);
				}
				handles.push_back(temp);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			for (int i = 0; i < mArgs.BatchSize; i++)
			{
				auto startTime = std::chrono::high_resolution_clock::now();
				Temp* heap = handles[i];
				/*if (HeapFree(heapHandle, 0, heap) == FALSE) {
					_tprintf(TEXT("Failed to free allocation from default process heap.\n"));
				}*/
				auto endTime = std::chrono::high_resolution_clock::now();
				auto executionTimeInMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
				addFreeTime(executionTimeInMs);
			}
		}

	}

	void PerformTest()
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < mArgs.MaxIterations; i++)
		{	
			std::cout << "Iteration " << (i+1) << std::endl;
			DWORD NumberOfHeaps;
			NumberOfHeaps = GetProcessHeaps(0, NULL);
			std::cout << "Number of heaps : " << NumberOfHeaps << std::endl;
			
			std::vector<std::thread> threads;
			threads.reserve(mArgs.MaxThreads);
			for (int i = 0; i < mArgs.MaxThreads; i++)
			{
				threads.push_back(std::thread(&HeapAllocationRunner::DoRun, this));
			}

			for (int i = 0; i < mArgs.MaxThreads; i++)
			{
				threads[i].join();
			}

			NumberOfHeaps = GetProcessHeaps(0, NULL);
			std::cout << "Number of heaps : " << NumberOfHeaps << std::endl;
		}
		auto endTime = std::chrono::high_resolution_clock::now();
		auto executionTimeInMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

		printResults();
		std::cout << "Total Time : " << executionTimeInMs.count() << "us" << std::endl;
	}

	HeapAllocationRunner(Arguments& args) : mArgs(args)
	{
		long long totalAllocations = mArgs.BatchIteration * mArgs.BatchSize * mArgs.MaxIterations * mArgs.MaxThreads;
		mAllocationTimings.reserve(totalAllocations);
		mFreeTimings.reserve(totalAllocations);

		if (!mArgs.UsePrivateHeap)
			hDefaultProcessHeap = HeapCreate(c_defaultDwFlags, c_initialHeapSize, c_maxHeapSize);
	}
};

int main(int argc, char* argv[])
{
	std::cout << "Scheduling Test!\n";
	Arguments args;
	args.parse(argc, argv);
	HeapAllocationRunner runner(args);
	runner.PerformTest();
	return 0;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
