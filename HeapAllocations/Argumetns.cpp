
#include "pch.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include "Arguments.h"

const std::string MAX_THREADS_ARG("/maxThreads");
const std::string MAX_ITERATIONS_ARG("/maxIterations");
const std::string BATCH_SIZE_ARG("/batchSize");
const std::string BATCH_ITERATIONS_ARG("/batchIterations");
const std::string USE_PRIVATE_HEAP_ARG("/privateHeap");

void Arguments::parse(int argc, char** argv)
{
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			std::string argument(argv[i]);
			size_t position = argument.find(':');
			if (position == -1)
			{
				std::cout << "Skipping invalid argument : " << argv[i] << std::endl;
				continue;
			}
			std::string argumentName = argument.substr(0, position);
			std::string argumentValue = argument.substr(position + 1);
			if (argumentName == MAX_THREADS_ARG)
			{
				MaxThreads = std::stol(argumentValue);
			}
			else if (argumentName == MAX_ITERATIONS_ARG)
			{
				MaxIterations = std::stol(argumentValue);
			}
			else if (argumentName == BATCH_SIZE_ARG)
			{
				BatchSize = std::stol(argumentValue);
			}
			else if (argumentName == BATCH_ITERATIONS_ARG)
			{
				BatchIteration = std::stol(argumentValue);
			}
			else if (argumentName == USE_PRIVATE_HEAP_ARG)
			{
				UsePrivateHeap = (argumentValue == "true");
			}
			else
			{
				std::cout << "Skipping invalid argument : " << argv[i] << std::endl;
			}
		}
	}

	std::cout << "\tConfiguration:" << std::endl;
	std::cout << "\t\t Max Threads: " << MaxThreads << std::endl;
	std::cout << "\t\t Max Iterations: " << MaxIterations << std::endl;
	std::cout << "\t\t Batch Size: " << BatchSize << std::endl;
	std::cout << "\t\t Batch Iterations: " << BatchIteration << std::endl;
	std::cout << "\t\t Use Private Heap: " << (UsePrivateHeap ? "true" : "false") << std::endl << std::endl << std::endl;
}


