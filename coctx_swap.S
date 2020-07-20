.globl coctx_swap
#if !defined( __APPLE__ )
.type  coctx_swap, @function
#endif
coctx_swap:
	leaq (%rsp),%rax
    movq %rax, 56(%rdi)
    movq %rbx, 48(%rdi)
	movq 0(%rax), %rax
	movq %rax, 40(%rdi) 
    movq %rbp, 32(%rdi)
    movq %r12, 24(%rdi)
    movq %r13, 16(%rdi)
    movq %r14, 8(%rdi)
    movq %r15, (%rdi)
	xorq %rax, %rax

    movq 32(%rsi), %rbp
    movq 56(%rsi), %rsp
    movq (%rsi), %r15
    movq 8(%rsi), %r14
    movq 16(%rsi), %r13
    movq 24(%rsi), %r12
    movq 48(%rsi), %rbx
	leaq 8(%rsp), %rsp
	pushq 40(%rsi)

	ret
