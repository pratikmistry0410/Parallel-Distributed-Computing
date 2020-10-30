#include <immintrin.h> 			// AVX
#include <stdio.h>
#include "timing.h"
#include <iostream>

using namespace std;

double randomNumber(int upper){
	return (double)rand() / RAND_MAX * upper;
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
    printf("Minimum cycles taken to calulate root via Manual AVX Intrinsics \t %f million cycles\n\n", minAVX);

  	return 0;
}
