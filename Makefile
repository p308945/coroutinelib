all : test.symmetric test.asymmetric test.copool

.PHONY : all

test.symmetric : test.symmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.symmetric.cpp -g -o test.symmetric

test.asymmetric : test.asymmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.asymmetric.cpp -g -o test.asymmetric

test.copool : test.copool.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.copool.cpp -g -o test.copool

clean :
	rm -rf test.symmetric test.asymmetric test.copool
