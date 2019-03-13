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
class HeapAllocationRunner
{
private:
	class Temp
	{
		long x;
		int y;
		double z;
	};

	Reliability::ReplicationAllocator<Temp>* pDefaultAllocator;
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
	
	void DoRun()
	{
		SIZE_T bytesToAllocate = 64 * 1024;
		
		Reliability::ReplicationAllocator<Temp>* pAllocator = pDefaultAllocator;
		if (mArgs.UsePrivateHeap)
		{
			std::shared_ptr<Reliability::IAllocationPolicy> pH = std::make_shared<Reliability::PrivateHeapAllocationPolicy>();
			pAllocator = new Reliability::ReplicationAllocator<Temp>(pH);
		}

		for (int j = 0; j < mArgs.BatchIteration; j++)
		{
			const std::thread::id threadId = std::this_thread::get_id();
			
			std::vector<Temp*, Reliability::ReplicationAllocator<Temp>> handles(*pAllocator);
			handles.reserve(mArgs.BatchSize);
			for (int i = 0; i < mArgs.BatchSize; i++)
			{
				auto startTime = std::chrono::high_resolution_clock::now();
				void* heap = (void*)pAllocator->allocate(bytesToAllocate);
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
				pAllocator->deallocate(heap);
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
		{
			std::shared_ptr<Reliability::IAllocationPolicy> pH = std::make_shared<Reliability::PrivateHeapAllocationPolicy>();
			pDefaultAllocator = new Reliability::ReplicationAllocator<Temp>(pH);
		}
	//		hDefaultProcessHeap = HeapCreate(c_defaultDwFlags, c_initialHeapSize, c_maxHeapSize);
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
