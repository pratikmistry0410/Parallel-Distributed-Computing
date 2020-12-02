all : bubble2 bubblesort_pthread

bubblesort_pthread: bubblesort_pthread.c
	gcc -g -Wall -o bubblesort_pthread bubblesort_pthread.c -lpthread

bubble2: bubble2.cpp
	g++ -std=c++11 -o bubble2 bubble2.cpp -pthread