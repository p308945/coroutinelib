#ifndef __CTX_SWAP_H__
#define __CTX_SWAP_H__

#include <stdlib.h>
// -----------

#define R15 0
#define R14 1
#define R13 2
#define R12 3
#define RBP 4
#define RIP 5
#define RBX 6
#define RSP 7

//-------------
// 64 bit
// low | regs[0]: r15 |
//    | regs[1]: r14 |
//    | regs[2]: r13 |
//    | regs[3]: r12 |
//    | regs[4]: rbp |
//    | regs[5]: ret |  //ret func addr
//    | regs[6]: rbx |
//    | regs[7]: rdi |
// hig | regs[8]: rsp |
enum {
  kRETAddr = 5,
  kRDI = 7,
  kRSP = 8,
};

struct coctx_t
{
	void *regs[9];
	size_t ss_size;
	char *ss_sp;

};

typedef void *(*pfn_co_routine_t)( void * );

struct stroutine
{
	pfn_co_routine_t pfn;
	coctx_t ctx;
	void *arg;	
};

void co_resume(stroutine *cur, stroutine *next);

stroutine *co_init(pfn_co_routine_t pfn, void *arg = NULL, int stack_size = 128 * 1024);
void co_destroy(stroutine *co);
void co_yield(stroutine *cur);
void co_resume(stroutine *nco);

#endif
