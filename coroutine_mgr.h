#ifndef __COROUTINE_MGR__
#define __COROUTINE_MGR__

#include <algorithm>
#include <map>
#include <vector>
#include <stdint.h>
#include <sys/time.h>
#include "ctx_swap.h"

static inline uint64_t coroutine_time_ms()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

struct CoTask;

void coroutine_sleep(CoTask *task, int ms);

bool coroutine_sleep2(int ms);

struct CoTaskTimerElem
{
	CoTaskTimerElem() : mCallbackID(-1), mTimeOut(0) {}
	CoTaskTimerElem(int callbackid, uint64_t timeout_ms) : mCallbackID(callbackid), mTimeOut(timeout_ms) {}
	uint64_t GetTimeOut() const
	{
		return mTimeOut;
	}
	int mCallbackID;
	uint64_t mTimeOut;
};

class CoroutineAsyncMap
{
	public:
		~CoroutineAsyncMap();
		CoTask *Alloc();
		void Free(CoTask *&task);
		CoTask *Find(int callbackid);

		int Tick();

		static CoroutineAsyncMap &GetInstance()
		{
			static CoroutineAsyncMap ins;
			return ins;
		}

		uint64_t GetCurMs()
		{
			return mCurMs;
		}

		int AddTimer(const CoTaskTimerElem &elem);

	private:
		CoroutineAsyncMap();
		int GetCallBackID();

		std::map<int32_t, CoTask *> mCoTaskMap;
		int32_t mCallbackID;
		uint64_t mCurMs;
		std::vector<CoTaskTimerElem> mTaskTimerHeap;
		uint32_t mTaskTimerCnt;
};

#endif

