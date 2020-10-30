#include <iostream>
#include <stdio.h>
#include <cmath>
#include "square_root_ispc.h"
#include <stdlib.h>
#include "timing.h"

using namespace std;
using namespace ispc;


double randomNumber(int upper){
	return (double)rand() / RAND_MAX * upper;
}

int main(int argc, char *argv[]){
	const unsigned int totalNum = 20 * 1e6;
	const float firstGuess = 2.0f;
	double minISPC = 1e30;
	float* values = new float[totalNum];
	float* result = new float[totalNum];
	// generate N float num;
	for(int i = 0; i < totalNum; i++){
		values[i] = randomNumber(9);
		result[i] = 0.0;
	}

	printf("\n");
	for(unsigned int i = 0; i < 3; i++){
		reset_and_start_timer();
		calulateRoot_ispc(totalNum, values, result, firstGuess);
		double dt = get_elapsed_mcycles();
    	printf("Cycles taken in iteration: %d for square root calculation via ISPC (single core):\t %.3f million cycles\n", i+1, dt);
        minISPC = std::min(minISPC, dt); 
	}
    printf("Minimum cycles taken to calulate root via ISPC (Single Core):\t %.3f million cycles\n\n", minISPC);

    delete[] values;
    delete[] result;
	return 0;
}