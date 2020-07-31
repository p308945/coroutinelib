#include "coroutine_mgr.h"

#define CO_TASK_TIMER_SIZE 10000

struct CoTask
{
	CoTask()
	{
		mCo = co_getcur();
		mUserPtr = NULL;
		mCallbackID = -1;
	}

	~CoTask() {}

	stroutine *mCo;
	void *mUserPtr;
	int mCallbackID;

	private:

	CoTask(const CoTask &);
	CoTask& operator = (CoTask &);
};

struct CoTaskTimerElemCmp
{
	bool operator()(const CoTaskTimerElem &l, const CoTaskTimerElem &r)
	{
		return l.GetTimeOut() > r.GetTimeOut();
	}
};

void coroutine_sleep(CoTask *task, int ms)
{
	CoTaskTimerElem elem(task->mCallbackID, CoroutineAsyncMap::GetInstance().GetCurMs() + ms);
	CoroutineAsyncMap::GetInstance().AddTimer(elem);
	co_yield(task->mCo);
}

bool coroutine_sleep2(int ms)
{
    CoTask *task = CoroutineAsyncMap::GetInstance().Alloc();
    if (NULL == task)
    {
        fprintf(stderr, "alloc task fail");
        return false;
    }
    CoTaskTimerElem elem(task->mCallbackID, CoroutineAsyncMap::GetInstance().GetCurMs() + ms);
    CoroutineAsyncMap::GetInstance().AddTimer(elem);
    co_yield(task->mCo);
    CoroutineAsyncMap::GetInstance().Free(task);
    return true;
}

CoroutineAsyncMap::CoroutineAsyncMap()
{
	mCallbackID = 0;
	mTaskTimerHeap.resize(CO_TASK_TIMER_SIZE);
	mTaskTimerCnt = 0;
}

CoroutineAsyncMap::~CoroutineAsyncMap()
{
	for (auto iter = mCoTaskMap.begin(); iter != mCoTaskMap.end(); ++iter)
	{
		delete iter->second;
	}
	mCoTaskMap.clear();
	mTaskTimerHeap.clear();
}

CoTask *CoroutineAsyncMap::Alloc()
{
	int callbackid = GetCallBackID();
	if (callbackid < 0)
	{
		return NULL;
	}

	CoTask *task = new CoTask();
	task->mCallbackID = callbackid;
	mCoTaskMap[callbackid] = task;
	return task;
}

void CoroutineAsyncMap::Free(CoTask *&task)
{
	mCoTaskMap.erase(task->mCallbackID);
	delete task;
	task = NULL;
}

CoTask *CoroutineAsyncMap::Find(int callbackid)
{
	auto iter = mCoTaskMap.find(callbackid);
	if (mCoTaskMap.end() == iter)
	{
		return NULL;
	}
	return iter->second;
}

int CoroutineAsyncMap::GetCallBackID()
{
	if (mCallbackID < 1)
	{
		mCallbackID = 1;
	}

	auto iter = mCoTaskMap.find(mCallbackID);
	int cnt = 0;
	while (iter != mCoTaskMap.end())
	{
		++cnt;
		if (cnt >= 100)
		{
			return -1;
		}
		++mCallbackID;
		if (mCallbackID < 1)
		{
			mCallbackID = 1;
		}
		iter = mCoTaskMap.find(mCallbackID);
	}
	return mCallbackID++;
}

int CoroutineAsyncMap::Tick()
{
	mCurMs = coroutine_time_ms();

	int cnt = 0;

	while (mTaskTimerCnt > 0 && mTaskTimerHeap[0].GetTimeOut() <= mCurMs)
	{
		++cnt;
		CoTaskTimerElem timer = mTaskTimerHeap[0];
		std::pop_heap(&mTaskTimerHeap[0], &mTaskTimerHeap[mTaskTimerCnt], CoTaskTimerElemCmp());
		--mTaskTimerCnt;

		CoTask *task = Find(timer.mCallbackID);
		if (NULL == task)
		{
			//already process
		}
		else
		{
			fprintf(stderr,"timeout %d, %lu\n", timer.mCallbackID, timer.GetTimeOut());
			co_resume(task->mCo);
		}
	}
	return cnt;
}

int CoroutineAsyncMap::AddTimer(const CoTaskTimerElem &elem)
{
	if (mTaskTimerCnt >= CO_TASK_TIMER_SIZE)
	{
		fprintf(stderr,"AddTimer error cnt %d\n", mTaskTimerCnt);
		return -1;
	}

	mTaskTimerHeap[mTaskTimerCnt] = elem;
	++mTaskTimerCnt;
	std::push_heap(&mTaskTimerHeap[0], &mTaskTimerHeap[mTaskTimerCnt], CoTaskTimerElemCmp());
	return mTaskTimerCnt;
}
