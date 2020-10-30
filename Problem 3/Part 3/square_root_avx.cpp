#include <immintrin.h> 			// AVX
#include <nmmintrin.h>          //SSE4.2
#include <stdio.h>
#include "timing.h"
#include <iostream>

using namespace std;

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

int main() {
	const unsigned int totalNum = 20 * 1e6;
	const float firstGuess = 2.0f;
    double minAVX = 1e30;
  	float* values = (float*)malloc(totalNum*sizeof(float));
	float* result = (float*)malloc(totalNum*sizeof(float));

	for(int i = 0; i < totalNum; i++){
		values[i] = randomNumber(9);
		result[i] = 0.0;
	}

    __m256 valuesVec = _mm256_loadu_ps(values);
    __m256 resultVec = _mm256_loadu_ps(result);

    printf("\n");
	for(unsigned int i = 0; i < 3; i++){
		reset_and_start_timer();
        resultVec  =_mm256_sqrt_ps(valuesVec);          //calculating the square root
		double dt = get_elapsed_mcycles();
    	printf("Cycles taken in iteration: %d for square root calculation via AVX Intrinsics:\t %.3f million cycles\n", i+1, dt);
	    minAVX = std::min(minAVX, dt); 
	}
    printf("Minimum cycles taken to calulate root via AVX Intrinsics \t %.3f million cycles\n\n", minAVX);

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

    printf("Total speedup from AVX Intrinsics to serial execution is: %.2fx speedup)\n", (minSerial/minAVX));

    printf("------------------------------------------------------------------------------------------------------------\n");
 
  return 0;
}
