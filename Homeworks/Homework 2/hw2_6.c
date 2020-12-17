#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#define numTasks 100
#define numWorkers 4

int output[numTasks];
int arr[numTasks];
pthread_mutex_t outputLock;
pthread_mutex_t taskLock;
int nextTask;

int isPrime( int n ) {
	int i;
	for ( i = 2; i <= ceil(sqrt(n)); i++ ) {
		if ( n%i == 0 ) {
			return 0; // composite
		}
	}
	return 1; // prime
}

int reversiblePrime( int n ) {
	int dig1 = n / 100;
	int dig2 = (n % 100) / 10;
	int dig3 = n % 10;
	int reverse = 100*dig3 + 10*dig2 + dig1;
	if ( isPrime(reverse) ) {
		return 1;
	} else {
		return 0;
	}
}

int getOutput( int n ) {
	if ( isPrime(n) ) {
		if ( reversiblePrime(n) ) {
			return 2; // prime forwards and backwards
		} else {
			return 1; // prime
		}
	} else {
		return 0; // composite
	}
}

void* worker1( void *passArg ) {
	int ind = *((int*)passArg);
	int num = arr[ind];
	int out = getOutput(num);

	pthread_mutex_lock(&outputLock);
	output[ind] = out;
	pthread_mutex_unlock(&outputLock);
}

void part1() { // #1: Generate 100 pthreads
	struct timeval startTime, endTime;
	gettimeofday(&startTime,NULL);

	pthread_t thread;
	int i;
	int threadNum[numTasks];
	for ( i = 0; i < numTasks; i++ ) {
		threadNum[i] = i;
		pthread_create(&thread, NULL, &worker1, &threadNum[i]);
		pthread_join(thread, NULL);
	}

	gettimeofday(&endTime,NULL);
	float totalTime = (1000000 * endTime.tv_sec + endTime.tv_usec) - (1000000 * startTime.tv_sec + startTime.tv_usec);
	printf("Version 1 runtime (in microseconds): %f\n",totalTime);
}

void* worker2( void *passArg ) {
	int ind = *((int*)passArg);
	int startIndex = ind*25;
	int endIndex = (ind+1)*25;
	int i;

	for ( i = startIndex; i < endIndex; i++ ) {
		int num = arr[i];
		int out = getOutput(num);
		pthread_mutex_lock(&outputLock);
		output[i] = out;
		pthread_mutex_unlock(&outputLock);
	}
}

void part2() { // #2: Generate 4 pthreads, 1 per block
	struct timeval startTime, endTime;
	gettimeofday(&startTime,NULL);

	int i;
	int threadNum[numWorkers];
	pthread_t thread[numWorkers];
	for ( i = 0; i < numWorkers; i++ ) {
		threadNum[i] = i;
		pthread_create(&thread[i], NULL, &worker2, &threadNum[i]);
	}
	for ( i = 0; i < numWorkers; i++ ) {
		pthread_join(thread[i], NULL);
	}

	gettimeofday(&endTime,NULL);
	float totalTime = (1000000 * endTime.tv_sec + endTime.tv_usec) - (1000000 * startTime.tv_sec + startTime.tv_usec);
	printf("Version 2 runtime (in microseconds): %f\n",totalTime);
}

void* worker3( void *passArg ) {
	int currTask;
	int num;

	while ( nextTask < numTasks ) {
		pthread_mutex_lock(&taskLock);
		if ( nextTask < numTasks ) { // another check..
			currTask = nextTask; // index to check 
			num = arr[currTask];
			nextTask++;
		}
		pthread_mutex_unlock(&taskLock);
		int out = getOutput(num);
		pthread_mutex_lock(&outputLock);
		output[currTask] = out;
		pthread_mutex_unlock(&outputLock);
	}
}

void part3() { // #3: Generate 4 pthreads, take next available element
	struct timeval startTime, endTime;
	gettimeofday(&startTime,NULL);

	int i;
	pthread_t thread[numWorkers];
	nextTask = 0;

	for ( i = 0; i < numWorkers; i++ ) {
		pthread_create(&thread[i], NULL, &worker3, NULL);
	}
	for ( i = 0; i < numWorkers; i++ ) {
		pthread_join(thread[i], NULL);
	}

	gettimeofday(&endTime,NULL);
	float totalTime = (1000000 * endTime.tv_sec + endTime.tv_usec) - (1000000 * startTime.tv_sec + startTime.tv_usec);
	printf("Version 3 runtime (in microseconds): %f\n",totalTime);
}

void oddRand() { // return odd number between 400-800
	int i;
	for ( i = 0; i < numTasks; i++ ) {
		arr[i] = ( rand() % 200 ) * 2 + 401;
	}
}

int main(int argc, char** argv) {
	if ( pthread_mutex_init(&outputLock, NULL) != 0 ) {
		exit(1); // mutex failed
	}
	if ( pthread_mutex_init(&taskLock, NULL) != 0 ) {
		exit(1); // mutex failed
	}

	oddRand(); // populate arr with random numbers

	// UNCOMMENT ONE OF THE FOLLOWING AT A TIME:
	//part1();
	//part2();
	//part3();

	pthread_mutex_destroy(&outputLock); // destroy mutex
	pthread_mutex_destroy(&taskLock);
	return 0;
} 