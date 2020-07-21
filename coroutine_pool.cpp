#include <stack>
#include <functional>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include "coroutine_pool.h"
#include "ctx_swap.h"

class CoWorker
{
	public:
		CoWorker(stroutine *co = NULL) : mpCo(co), mFunc(NULL), mExit(false) {}

		stroutine *mpCo;
		std::function<void(void)> mFunc;
		typedef void(*funType)(void);
		bool mExit;
};

class CoWorkerPool
{
	public:
		CoWorkerPool()
		{
			mMinStackSize = default_min_stack_size;
			mStackSize = default_stack_size;
			mFreeSize = default_stack_size - default_min_stack_size;
		}

		std::stack<CoWorker *> mWorksPool;
		int mMinStackSize;
		int mStackSize;
		int mFreeSize;
};

static CoWorkerPool g_coworker_pool;

void InitCoWorkerPool(int min_stack_size, int stack_size)
{
	static bool has_init = false;
	if (has_init)
	{
		return;
	}

	has_init = true;

	if (min_stack_size < default_min_stack_size)
	{
		min_stack_size = default_min_stack_size;
	}

	if (stack_size > default_stack_size)
	{
		stack_size = default_stack_size;
	}

	if (stack_size < min_stack_size)
	{
		stack_size = min_stack_size;
	}

	long pageSize = sysconf(_SC_PAGESIZE);
	min_stack_size = (min_stack_size + pageSize - 1) / pageSize * pageSize;
	stack_size = (stack_size + pageSize - 1) / pageSize * pageSize;

	g_coworker_pool.mMinStackSize = min_stack_size;
	g_coworker_pool.mStackSize = stack_size;
	g_coworker_pool.mFreeSize = stack_size - min_stack_size;
}

static void freeRss(char *stack_sp)
{
	long pageSize = sysconf(_SC_PAGESIZE);
	char *tmp = stack_sp + g_coworker_pool.mFreeSize - pageSize;
	void *pageStart = (void *)((long long)tmp & ~(pageSize - 1));
	unsigned char vec[1] = {0};
	int ret = mincore(pageStart, pageSize, vec);
	if (ret != 0)
	{
		//log error
		fprintf(stderr, "mincore fail ret = %d\n", ret);
		return;
	}

	if ((vec[0] & 0x1) == 0)
	{
		return;
	}

	ret = madvise(stack_sp, g_coworker_pool.mFreeSize, MADV_DONTNEED);
	if (ret != 0)
	{
		//log error
		fprintf(stderr, "miadvice fail ret = %d\n", ret);
		return;
	}

	return;
}

static void *worker_main(void *arg)
{
	CoWorker *worker = (CoWorker *)arg;
	while(!worker->mExit)
	{
		if (worker->mFunc)
		{
			worker->mFunc();
		}
		worker->mFunc = NULL;
		if (g_coworker_pool.mFreeSize > 0)
		{
			freeRss(worker->mpCo->ctx.ss_sp);
		}
		g_coworker_pool.mWorksPool.push(worker);
		co_yield(worker->mpCo);
	}
}

void ProcessByCoWorker(std::function<void(void)> func)
{
	CoWorker *worker = NULL;
	if (g_coworker_pool.mWorksPool.empty())
	{
		worker = new CoWorker();
		stroutine *worker_routine = co_init(worker_main, worker, default_stack_size);
		worker->mpCo = worker_routine;
	}
	else
	{
		worker = g_coworker_pool.mWorksPool.top();
		g_coworker_pool.mWorksPool.pop();
	}
	if (NULL == worker)
	{
		//log error
		fprintf(stderr, "%s %d get worker fail\n");
		return;
	}
	worker->mFunc = func;
	co_resume(worker->mpCo);
}
