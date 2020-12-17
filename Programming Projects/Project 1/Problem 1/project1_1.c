#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// to use for stride size, array size later
#define kb 1024
#define mb 1024*1024

void initializeData() {
	return;
}

void testCacheSize() {
	// test with data re-use

	FILE *fptr; //output text file
    fptr = fopen("arrSize2.txt", "ab+");

	int strides[] = { 16, 64, 256, 1*kb, 4*kb, 16*kb, 64*kb }; // test strides

	int arrSizes[] = { // test these array sizes
		16*kb/4, 32*kb/4, 64*kb/4, 128*kb/4, 256*kb/4, 512*kb/4, 1*mb/4, 2*mb/4, 4*mb/4
	};


	int arr[mb]; // max size allowed without seg fault

	int dummy = 0;
	int arrSize, strideSize, times, i, stride;
	struct timeval stop, start;

	int steps = 128 * 1024 * 1024; // arbitrary # steps to test accessing same data

	for ( strideSize = 0; strideSize < sizeof(strides)/sizeof(int); strideSize++ ) { // take different strides
		stride = strides[strideSize];
		for ( arrSize = 0; arrSize < sizeof(arrSizes)/sizeof(int); arrSize++ ) {
			// using (i & mod) gives better results than (i % len)
			int len = arrSizes[arrSize];
			int mod = len - 1;

			gettimeofday(&start, NULL);
		    for (i = 0; i < steps; i+=stride) { // take different amts of strides
			    dummy = arr[i & mod]; // when i >= len, loop back around to beginning
			}
			gettimeofday(&stop, NULL);
			float totalTime = (1000000 *stop.tv_sec + stop.tv_usec) - (1000000 * start.tv_sec + start.tv_usec); // microseconds
			fprintf(fptr, "%d     %f\n", arrSizes[arrSize]*4/kb, totalTime);
		}
	}
		
    fclose(fptr); //close file
	return;
}

void testBandwidth() {
	// test with no data re-use
	// for different array sizes:
	// START TIMER
	// for each element in array:
	//     touch (with dummy variable)
	// END TIMER
	// divide array size by time

	struct timeval stop, start;
	FILE *fptr; //output text file
    fptr = fopen("bandwidth.txt", "ab+");
	int arrSizes[] = { // test these array sizes
		16*kb/4, 32*kb/4, 64*kb/4, 128*kb/4, 256*kb/4, 512*kb/4, 1*mb/4, 2*mb/4, 4*mb/4
	};

	int arrSize, i, dummy;
	for ( arrSize = 0; arrSize < sizeof(arrSizes)/sizeof(int); arrSize++ ) {
		int length = arrSizes[arrSize];
		int arr[length];
		gettimeofday(&start, NULL);
		for ( i = 0; i < length; i++) { // collect time to access each element in array
			dummy = arr[i];
		}
		gettimeofday(&stop, NULL);
		float totalTime = (1000000 *stop.tv_sec + stop.tv_usec) - (1000000 * start.tv_sec + start.tv_usec); // microseconds
		float bandwidth = (length*4)/totalTime*10e6/(1024*1024*1024);
		fprintf(fptr, "%d     %f\n", length*4/kb, bandwidth);
	}

	fclose(fptr); //close file
	return;
}

int main(int argc, char** argv) {
	printf("starting tests\n");
	testCacheSize();
	testBandwidth();
	printf("completed. check output files.\n");
	return 0;
}