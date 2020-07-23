all : test.symmetric test.asymmetric test.copool libcoroutine.a libcoroutine.so tags

.PHONY : all

test.symmetric : test.symmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.symmetric.cpp -g -o test.symmetric

test.asymmetric : test.asymmetric.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.asymmetric.cpp -g -o test.asymmetric

test.copool : test.copool.cpp coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp test.copool.cpp -g -o test.copool

libcoroutine.a : coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -c -std=c++0x coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	ar cr $@ ctx_swap.o coroutine_pool.o

libcoroutine.so : coctx_swap.S ctx_swap.cpp coroutine_pool.cpp
	g++ -std=c++0x -shared -fPIC coctx_swap.S ctx_swap.cpp coroutine_pool.cpp -o $@

tags :
	ctags -R *

clean :
	rm -rf test.symmetric test.asymmetric test.copool libcoroutine.a libcoroutine.so tags *.o 
