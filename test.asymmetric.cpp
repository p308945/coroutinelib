#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include "ctx_swap.h"

stroutine *ro1 = NULL;
stroutine *ro2 = NULL;
stroutine ro0;
bool co1Exit = false;
bool co2Exit = false;
int idx = 1;

void* fun1(void *)
{
	int i = 100000000;
	while(i--)
	{
	//	std::cerr << "in fun1" << std::endl;
		if (NULL != ro2)
		{
			idx = 2;
			co_yield(ro1);
//			co_resume(ro1, ro2);
		}
	}
	std::cerr << "in fun1 " << i << std::endl;
	co1Exit = true;
	co_yield(ro1);
//	co_resume(ro1, ro2);
	return NULL;
}

void* fun2(void *)
{
	int i = 100000000;
	while(i--)
	{
	//	std::cerr << "in fun2" << std::endl;
		if (NULL != ro1)
		{
			idx = 1;
	//		co_resume(ro2, ro1);
			co_yield(ro2);
		}
	}
	std::cerr << "in fun2 " << i << std::endl;
//	co_resume(ro2, &ro0);
	co2Exit = true;
	co_yield(ro2);
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
	co_resume(ro1);
	stroutine *ro = NULL;
	while (true)
	{
		if (idx == 2 && !co2Exit)
		{
			ro = ro2;
		}
		else if (idx == 1 && !co1Exit)
		{
			ro = ro1;
		}
		else
		{
			if (co1Exit)
			{
				ro = ro2;
			}
			if (co2Exit)
			{
				ro = ro1;
			}
		}
		if (co1Exit && co2Exit)
		{
			break;
		}
		co_resume(ro);
	}
	gettimeofday(&tv_end, NULL);
	std::cerr << 1000000 *(tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec) 
		<< "us" << std::endl;
	co_destroy(ro1);
	co_destroy(ro2);
	return 0;
}
