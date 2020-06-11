a.out : test.c coctx_swap.S ctx_swap.c
	g++ test.c coctx_swap.S ctx_swap.c -g

clean :
	rm -rf a.out
