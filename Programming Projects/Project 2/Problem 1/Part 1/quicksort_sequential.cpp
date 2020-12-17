//Sequential Quick Sort Program

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <random>

using namespace std;


//Generation of the random array
double* generate_array(double* list, int num) {
  random_device rd;
  mt19937 eng(rd());
  uniform_real_distribution<> distr(0, 100);

  for (int i = 0; i < num; i++) {
    list[i] = distr(eng);
  }

  return list;
}

//Quick sort sequential algorithm
void qsort_seq(double * list, int l, int r) {
  int i = l, j = r;
  double temp;
  double pivot = list[(l + r) / 2];
  
  //After calculation of the pivot we will run the partition algorithm in order to put the pivot in right position and then separating the array into left and right 
  while (i <= j) {
    while (list[i] < pivot)
      i++;
    while (list[j] > pivot)
      j--;
    if (i <= j) {
      temp = list[i];
      list[i] = list[j];
      list[j] = temp;
      i++;
      j--;
    }
  }

 // Then we again using recursion call the quick sort sequential on the left and the right parts
  if (l < j)
    qsort_seq(list, l, j);
  if (i < r)
    qsort_seq(list, i, r);
}

//Main function for calling the sequential sort
int main(int argc, char *argv[]) {

  if (argc != 2){
		fprintf(stderr, "Please enter N i.e. Number of elements in list\n");
		exit(0);
	}

  //Based on the number input with the list of array the dynamic array is created
  int num = atoi(argv[1]);
  double* list = new double[num];
  
  //List is created
  list = generate_array(list, num);

  //Time calculation
  clock_t start, end;
  start = clock();
  qsort_seq(list , 0, num - 1);
  end = clock();
  
  //Output of the time
  float secs = ((float)end - (float)start) / CLOCKS_PER_SEC;
  cout << "Time taken by sequential quicksort is = " << secs << " seconds" << endl;

  return 0 ;

}