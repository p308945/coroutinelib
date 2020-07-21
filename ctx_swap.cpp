#include <string.h>
#include "ctx_swap.h"

static stroutine mainCo;

static void routineFunc(stroutine *co)
{
	if (co->pfn)
	{
		co->pfn(co->arg);
	}
}

static int ctx_make(stroutine *pc, pfn_co_routine_t pfn)
{
	coctx_t *ctx = &(pc->ctx);
	char *sp = ctx->ss_sp + ctx->ss_size;
	sp = (char *)((unsigned long)sp & -16LL);
	memset(ctx->regs, 0x0, sizeof(ctx->regs));
	void **ret_addr = (void **)(sp);
	*ret_addr = (void *)pfn;
	ctx->regs[kRDI] = pc->arg;
	ctx->regs[kRSP] = sp;
	ctx->regs[kRETAddr] = (char *)pfn;
	return 0;
}


stroutine *co_init(pfn_co_routine_t pfn, void *arg, int stack_size)
{
	stroutine *pc = (stroutine *)malloc(sizeof(stroutine));
	memset(pc, 0, sizeof(stroutine));
	pc->pfn = pfn;
	pc->arg = arg;
	pc->ctx.ss_size = stack_size;
	pc->ctx.ss_sp = (char *)malloc(stack_size);
	ctx_make(pc, pfn);
	return pc;
}

extern "C"
{
	extern void coctx_swap( coctx_t *,coctx_t* ) asm("coctx_swap");
};

void co_resume(stroutine *cur, stroutine *next)
{
	coctx_swap(&(cur->ctx), &(next->ctx));
}

void co_destroy(stroutine *co)
{
	if (NULL != co)
	{
		if (NULL != co->ctx.ss_sp)
		{
			free(co->ctx.ss_sp);
			co->ctx.ss_sp = NULL;
		}
		free(co);
	}
}

void co_yield(stroutine *cur)
{
	co_resume(cur, &mainCo);
	//todo
}

void co_resume(stroutine *nco)
{
	co_resume(&mainCo, nco);
}
