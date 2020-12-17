all : quicksort_pthreads bitonic_pthreads

quicksort_pthreads: quicksort_pthreads.cpp
	g++ -std=c++11 -o quicksort_pthreads quicksort_pthreads.cpp -pthread

bitonic_pthreads: bitonic_pthreads.c
	gcc -g -Wall -o bitonic_pthreads bitonic_pthreads.c -lpthread
