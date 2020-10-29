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

double calulateRoot(double input, double firstGuess){
	double x1 = -1.0;
    double x0 = -1.0;

    x0 = firstGuess;
    do{
        x1 = (x0 - (((x0 * x0) - input) / (2 * x0)));
        x0 = x1;
    }while((x1 * x1) - input > 1e-4);

    return x1;
}

void calulateRoot_serial(double count, float* input, float* output, float firstGuess){
	for(int i = 0; i < count; i ++){
		output[i] = calulateRoot(input[i], firstGuess);
	}
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

	for(unsigned int i = 0; i < 3; i++){
		reset_and_start_timer();
		calulateRoot_ispc(totalNum, values, result, firstGuess);
		double dt = get_elapsed_mcycles();
    	printf("Cyles taken in iteration: %d for square root calculation via ISPC (single core):\t\t\t %.3f million cycles\n", i, dt);
        minISPC = std::min(minISPC, dt); 
	}
    printf("Minimum cycles taken to calulate root via ISPC (Single Core):\t\t %.3f million cycles\n", minISPC);

	double minSerial = 1e30;
	for(unsigned int i = 0 ; i < 3; i ++){
		reset_and_start_timer();
		calulateRoot_serial(totalNum, values, result, firstGuess);
		double dt = get_elapsed_mcycles();
    	printf("Cyles taken in iteration: %d for square root calculation serially:\t\t\t %.3f million cycles\n", i, dt);
        minSerial = std::min(minSerial, dt); 
	}
    printf("Minimum cycles taken to calulate root serially:\t\t %.3f million cycles\n", minSerial);

    printf("\n Total speedup from ISPC with respect to serial execution is: %.2fx speedup)\n", minSerial/minISPC);

    delete[] values;
    delete[] result;
	return 0;
}