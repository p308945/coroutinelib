#ifndef __COROUTINE_POOL_H__
#define __COROUTINE_POOL_H__

static const int default_min_stack_size = 512 * 1024;
static const int default_stack_size = 8 * 1024 * 1024;

void InitCoWorkerPool(int min_stack_size = default_min_stack_size, int stack_size = default_stack_size);

void ProcessByCoWorker(std::function<void(void)> func);

#endif
