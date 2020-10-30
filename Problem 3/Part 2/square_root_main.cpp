#include <iostream>
#include <stdio.h>
#include <cmath>
#include "square_root_tasks_ispc.h"
#include <stdlib.h>
#include "timing.h"

using namespace std;
using namespace ispc;


double randomNumber(int upper){
	return (double)rand() / RAND_MAX * upper;
}

double calculateRoot(double input, double firstGuess){
	double x1 = -1.0;
    double x0 = -1.0;

    x0 = firstGuess;
    do{
        x1 = (x0 - (((x0 * x0) - input) / (2 * x0)));
        x0 = x1;
    }while((x1 * x1) - input > 1e-4);

    return x1;
}

void calculateRoot_serial(double count, float* input, float* output, float firstGuess){
	for(int i = 0; i < count; i ++){
		output[i] = calculateRoot(input[i], firstGuess);
	}
}

int main(int argc, char *argv[]){
    const unsigned int totalNum = 20 * 1e6;
	const float firstGuess = 2.0f;
	double minISPC = 1e30;
	float* values = new float[totalNum];
	float* result = new float[totalNum];

    if (argc < 2) {
        printf(stderr, "You must pass number of threads in arguments.\n");
        exit(EXIT_FAILURE);
    }

	const int nbThreads  = atoi(argv[1]);
	// generate N float num;
	for(int i = 0; i < totalNum; i++){
		values[i] = randomNumber(9);
		result[i] = 0.0;
	}

    printf("\n");
	for(unsigned int i = 0; i < 3; i++){
		reset_and_start_timer();
		calculateRoot_ispc_tasks(totalNum, values, result, firstGuess,nbThreads);
		double dt = get_elapsed_mcycles();
    	printf("Cycles taken in iteration: %d for square root calculation via ISPC with %d threads:\t %.3f million cycles\n", i+1, nbThreads, dt);
	    minISPC = std::min(minISPC, dt); 
	}
    printf("Minimum cycles taken to calulate root via ISPC with %d threads:\t %.3f million cycles\n\n", nbThreads, minISPC);

	double minSerial = 1e30;
	for(unsigned int i = 0 ; i < 3; i ++){
		reset_and_start_timer();
		calculateRoot_serial(totalNum, values, result, firstGuess);
		double dt = get_elapsed_mcycles();
    	printf("Cycles taken in iteration: %d for square root calculation serially:\t %.3f million cycles\n", i+1, dt);
        minSerial = std::min(minSerial, dt); 
	}
    printf("Minimum cycles taken to calulate root serially:\t %.3f million cycles\n", minSerial);

    printf("------------------------------------------------------------------------------------------------------------\n");

    printf("Total speedup from ISPC with %d threads compared to serial execution is: %.2fx speedup)\n", nbThreads, (minSerial/minISPC));

    printf("------------------------------------------------------------------------------------------------------------\n");

    delete[] values;
    delete[] result;
	return 0;
}




