#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <functional>
#include "ctx_swap.h"
#include "coroutine_pool.h"

void fun()
{
	char buf[600 * 1024] = {0};
	buf[0] = 1;
	//std::cerr << "in fun " << std::endl;
}

int main()
{
	std::cerr << "in main" << std::endl;
	InitCoWorkerPool();
	struct timeval tv_begin;
	struct timeval tv_end;
	gettimeofday(&tv_begin, NULL);
	long count = 100000000;
	while (count--)
	{
		ProcessByCoWorker(std::bind(fun));
	}
	gettimeofday(&tv_end, NULL);
	std::cerr << 1000000 *(tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec) 
		<< "us" << std::endl;
	return 0;
}
