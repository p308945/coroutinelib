#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include "ctx_swap.h"

stroutine *ro1 = NULL;
stroutine *ro2 = NULL;
stroutine ro0;

void* fun1(void *)
{
	int i = 100000000;
	while(i--)
	{
//		std::cerr << "in fun1" << std::endl;
		if (NULL != ro2)
		{
			co_resume(ro1, ro2);
		}
	}
	co_resume(ro1, &ro0);
	return NULL;
}

void* fun2(void *)
{
	int i = 100000000;
	while(i--)
	{
//		std::cerr << "in fun2" << std::endl;
		if (NULL != ro1)
		{
			co_resume(ro2, ro1);
		}
	}
	co_resume(ro2, &ro0);
	return NULL;
}

int main()
{
	std::cerr << "in main" << std::endl;
	ro1 = co_init(fun1);
	ro2 = co_init(fun2);
	struct timeval tv_begin;
	struct timeval tv_end;
	gettimeofday(&tv_begin, NULL);
	co_resume(&ro0, ro1);
	gettimeofday(&tv_end, NULL);
	std::cerr << 1000000 *(tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec) 
		<< "us" << std::endl;
	return 0;
}
