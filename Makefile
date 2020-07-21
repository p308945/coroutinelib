all : test.symmetric test.asymmetric

.PHONY : all

test.symmetric : test.symmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.symmetric.cpp -g

test.asymmetric : test.asymmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.asymmetric.cpp -g

clean :
	rm -rf test.symmetric test.asymmetric
