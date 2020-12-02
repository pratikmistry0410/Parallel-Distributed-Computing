all : bitonic_sequential bitonic_pthreads

bitonic_sequential: bitonic_sequential.cpp
	g++ -std=c++11 -o bitonic_sequential bitonic_sequential.cpp

bitonic_pthreads: bitonic_pthreads.c
	gcc -g -Wall -o bitonic_pthreads bitonic_pthreads.c -lpthread
