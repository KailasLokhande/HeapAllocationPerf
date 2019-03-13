#pragma once
constexpr long MAX_THREADS = 1;
constexpr long MAX_ITERATIONS = 1;
constexpr long BATCH_SIZE = 10;
constexpr long BATCH_ITERATIONS = 1;
constexpr bool USE_PRIVATE_HEAP = true;

struct Arguments
{
	long MaxThreads = MAX_THREADS;
	long MaxIterations = MAX_ITERATIONS;
	long BatchSize = BATCH_SIZE;
	long BatchIteration = BATCH_ITERATIONS;
	long UsePrivateHeap = USE_PRIVATE_HEAP;

	void parse(int argc, char** argv);
};