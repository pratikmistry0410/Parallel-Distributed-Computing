// Bitonic sort of a list of integers using sequential method

#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <random>
#define RMAX 1000000                                     // Random values in the range 0 to RMAX-1
using namespace std;


// Main Function - Driver Function
int main(int argc, char *argv[]) {
	if (argc != 2){
		fprintf(stderr, "Please enter N i.e. Number of elements in list\n");
		exit 0;
	}
	int num_elements = atoi(argv[1]);
	int* a = new int[num_elements];

	a = generate_list(a, num_elements);
	
	int up = 1;   // Variable to indicate that we want to sort in ascending order

	clock_t start, end;
	start = clock();

	sort(a, num_elements, up);

	end = clock();

	/*printf("Sorted array is: \n");
	for (int i = 0; i < num_elements; i++)
		cout << a[i] << endl;
	*/
	float seconds = ((float)end - (float)start) / CLOCKS_PER_SEC;
	cout << "Total time taken to sort the list by Bitonic Sort using Sequential Method = " << seconds << " seconds" << endl;

	return 0;
}

// Generate list of random numbers
int* generate_list(int* array, int num_elements) {
	int i;
   	srandom(1);
   	for (i = 0; i < num_elements; i++)
      array[i] = random() % RMAX;
	return array;
}


// Caller of bitonic_sort for sorting the entire array of length N in ASCENDING order
void sort(int a[], int N, int up)
{
	bitonic_sort(a, 0, N, up);
}


/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.*/
void compare_and_swap(int a[], int i, int j, int dir)
{
	if (dir == (a[i] > a[j]))
		swap(a[i], a[j]);
}


/* This function first produces a bitonic sequence by recursively
    sorting its two halves in opposite sorting orders, and then
    calls bitonic_merge to make them in the same order */
void bitonic_sort(int a[], int low, int count, int dir)
{
	if (count > 1)
	{
		int k = count / 2;

		// sort in ascending order since dir here is 1
		bitonic_sort(a, low, k, 1);

		// sort in descending order since dir here is 0
		bitonic_sort(a, low + k, k, 0);

		// Will merge wole sequence in ascending order
		// since dir=1.
		bitonic_merge(a, low, count, dir);
	}
}


/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter count is the number of elements to be sorted.*/
void bitonic_merge(int a[], int low, int count, int dir)
{
	if (count > 1)
	{
		int k = count / 2;
		for (int i = low; i < low + k; i++)
			compare_and_swap(a, i, i + k, dir);
		bitonic_merge(a, low, k, dir);
		bitonic_merge(a, low + k, k, dir);
	}
}