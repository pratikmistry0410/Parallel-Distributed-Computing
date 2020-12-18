#include <stdio.h>
#include <stdlib.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<ctime>
#include<iostream>

using namespace std;

#define N 300

double diff_time(clock_t clock1, clock_t clock2)
{
	double diff = clock1 - clock2;
	double diff_t = (diff * 10) / CLOCKS_PER_SEC;
	return diff_t;
}

__global__ void alignment(char* key, char* s, int* arr_scores, int n, int num)
{
	
	//This is the values assiged for gap , match and mismatch
	int GAP = -2, MATCH = 1, MISMATCH = -1;

	//This is the index of the element is defined in terms of the Thread ID , Block Id and Dimension of Block
	int index = threadIdx.x + blockIdx.x * blockDim.x;

	//This condition is to prevent that we do not go ahead from the num - number of strings
	if (index < num)
	{
		int i, j, k, dia, top, left;

		//Here the DP array and the traceback arrays are created with N+1 Length
		//Note DP array is the scoring matrix used in Needleman Wunsch algorithm
		int dp[N + 1][N + 1];

		//R1 and R2 are two aligned sequences
		char r1[2 * N + 2], r2[2 * N + 2];
		
		//Traceback array is for storing the traceability the position from where we reached at certain position in the scoring matrix
		char traceback[N + 1][N + 1];

		//Then the default values are filled in the top row and left column of the DP and Traceback matrix
		for (i = 0; i <= n; i++)
		{
			dp[0][i] = GAP * i;
			dp[i][0] = GAP * i;
			traceback[0][i] = 'l';
			traceback[i][0] = 'u';
		}

		//This is for the filling of the DP and Traceback Matrix
		for (i = 1; i <= n; i++)
		{
			for (j = 1; j <= n; j++)
			{   
				if (key[i - 1] == s[n * index + j - 1])
					dia = dp[i - 1][j - 1] + MATCH;
				else
					dia = dp[i - 1][j - 1] + MISMATCH;
				top = dp[i - 1][j] + GAP;
				left = dp[i][j - 1] + GAP;
				dp[i][j] = dia > top ? (dia > left ? dia : left) : (top > left ? top : left);
				traceback[i][j] = dp[i][j] == dia ? 'd' : (dp[i][j] == top ? 'u' : 'l');
			}
		}
	
		//Here we are tracebacking based on the position we have came from
		//If we came from left and upper that means gap so we fill that by - that is in case of left r1 and in case of up r2
		i = n, j = n, k = 0;
		while (!(i == 0 && j == 0))
		{
			if (traceback[i][j] == 'd')
			{
				r1[k] = key[i - 1];
				r2[k] = s[n * index + j - 1];
				i--;
				j--;
			}
			else if (traceback[i][j] == 'u')
			{
				r1[k] = key[i - 1];
				r2[k] = '-';
				i--;
			}
			else
			{
				r1[k] = '-';
				r2[k] = s[n * index + j - 1];
				j--;
			}
			k++;
		}

		for (i = 0; i < k / 2; i++)
		{
			r1[i] = (r1[i] + r1[k - i - 1]) - (r1[k - i - 1] = r1[i]);
			r2[i] = (r2[i] + r2[k - i - 1]) - (r2[k - i - 1] = r2[i]);
		}
		r1[k] = '\0';
		r2[k] = '\0';
        
		//Printing the aligned key and the query string
		printf("\nAlignment #%d :\n-------------------\nKey:\n%s\nQuery:\n%s\n", index + 1, r1, r2);
		
		//Calculated the score based on the alignment achieved using the algorithm
		int score = 0;
		for (i = 0; i < k; i++)
		{
			if (r1[k] == '-' || r2[k] == '-')
				score += GAP;
			else if (r1[i] == r2[i])
				score += MATCH;
			else
				score += MISMATCH;
		}
		
		//For each query string we assign the score
		arr_scores[index] = score;
	}
}

int main(int argc, char** argv)
{
	
	

    //Number of Threads Ceated	
	int THREADS = 1024;

	//Two files as arguments for acting as input and ouput for this program as the input strings and numbers and strings are given in files
	freopen(argv[1], "r", stdin);
	freopen(argv[2], "a", stdout);

	//Array pointers for scores
	int* h_scores, * scores;
	int i, num, n;

	//Taking inputs from the file - length of the string
	scanf("%d", &n);
    
	//Taking inputs from the file - number of strings
	scanf("%d", &num);

	//Here if n is smaller than number of Threads which is 1024 in our case then m is n else threads - 1024
	int m = n < THREADS ? n : THREADS;


	char* host_key = (char*)malloc(n);
	char* tmp = (char*)malloc(n);
	char* host_q = (char*)malloc(num * n + 2);
	char* key, * q;

	//Taking the host key from the file
	scanf("%s", host_key);


	//Taking the other strings from the file
	for (i = 0; i < num; i++)
	{
		if (i == 0)
			scanf("%s", host_q);
		else
		{
			scanf("%s", tmp);
			strcat(host_q, tmp);
		}
	}

	h_scores = (int*) malloc(sizeof(int) * num);

	//Here the memory refernces are created in device memory for scores , keys 
	cudaMalloc((void**)&scores, num * sizeof(int));
	cudaMalloc((void**)&key, n);
	cudaMalloc((void**)&q, n * num + 2);

	//Here the values are copied to the device - host_key and queried_strings 
	cudaMemcpy(key, host_key, n, cudaMemcpyHostToDevice);
	cudaMemcpy(q, host_q, n * num + 2, cudaMemcpyHostToDevice);

	// Then we started the timer here
	clock_t begin = clock();

	//Global Cuda function called
	//Here the number of blocks and threads are defined 
	alignment << <(n + m - 1) / m, m >> > (key, q, scores, n, num);
	clock_t end = clock();
	cout << "Time elapsed in Parallel : " << double(diff_time(end, begin)) << " ms" << endl;
	// End the timer here.....
	
	//Here we copy the scores from the device to program memory
	cudaMemcpy(h_scores, scores, sizeof(int) * num, cudaMemcpyDeviceToHost);

	//Then we print the alignment scores of each of the queried strings
	printf("\n\nAlignment Scores:\n----------------------------\n");
	for (i = 0; i < num; i++)
		printf("Query #%d : %d\n", i + 1, h_scores[i]);

	//Here we free the allocated memory from the device
	cudaFree(key);
	cudaFree(q);
	cudaFree(scores);
	return 0;
}