#include <iostream>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <random>
#include <sys/types.h>

//Macro defining the number of threads
#define N  3

using namespace std;

pthread_mutex_t mutexsum;


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

//Working data in a data structure for threads
struct data
{
  double* ptr ;
  int l_index;
  int r_index;
};

//These are the data structures for the thread data and the number of pthreads based on predefine number of threads Macro
struct data thread_array[N];

pthread_t threads[N];

//This is the seq quick sort
void qsort_seq(double * list, int l, int r) {

  int i = l, j = r;
  double temp;
  double pivot = list[(l + r) / 2];

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

  if (l < j)
    qsort_seq(list, l, j);
  else if (i < r)
    qsort_seq(list, i, r);
}

//This is the function called by the two threads for the left and the right
void* qsort_threaded(void * arg) {

  struct data * t_data;

  t_data = (struct data *) arg;

  int i = t_data->l_index , j = t_data->r_index ;

  double * ptr = t_data->ptr ;

  qsort_seq(ptr, t_data->l_index, t_data->r_index);

}

//This is the parallel quick sort called by the first thread
void* qsort_parallel(void * arg) {

  int r;

  void* status;

  struct data * t_data;

  //so the arguments passed are taken in the data structure for the thread
  t_data = (struct data *) arg;

  int i = t_data->l_index, j = t_data->r_index ;

  double * arr = t_data->ptr;

  double tmp;

  double pivot = arr[(i + j) / 2];

  unsigned int tid = (unsigned int)pthread_self();

  //This is the partition algorithm used in the quick sort

  while (i <= j) {
    while (arr[i] < pivot)
      i++;
    while (arr[j] > pivot)
      j--;
    if (i <= j) {
      tmp = arr[i];
      arr[i] = arr[j];
      arr[j] = tmp;
      i++;
      j--;
    }
  }

  // Then we generate the parallel threads for the left part and the right part
  //Passing the appropriate data structure

 if (t_data->l_index < j) {

    thread_array[1].ptr = arr;
    thread_array[1].l_index  = t_data->l_index;
    thread_array[1].r_index = j;

    int ret = pthread_create(&threads[1], NULL, qsort_threaded, (void *)&thread_array[1]);

    if (ret) {
      printf("Error code is %d\n", ret);
      exit(-1);
    }


    if (i < t_data->r_index) {

      thread_array[2].ptr = arr;
      thread_array[2].l_index = i;
      thread_array[2].r_index = t_data->r_index;

      int ret = pthread_create(&threads[2], NULL, qsort_threaded, (void *)&thread_array[2]);

      if (ret) {
        printf("ERROR code is %d\n", ret);
        exit(-1);
      }
    }
  }

  pthread_exit((void*) arg);
}


int main(int argc, char *argv[]) {


  //These are the number of elements to be sorted taken as an input with the program
  const int num  = atoi(argv[1]);

  //List is created with the above input number of elements
  double* list = new double[num];

  //For status
  void* status;

  //Start and end time of the parallel quicksort program
  clock_t start, end;


  pthread_mutex_init(&mutexsum, NULL);
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


  //Generate the List of random numbers
  list = generate_array(list, num);

  int ret;


  //First Data structure for the thread execution is filled
  thread_array[0].ptr = list;
  thread_array[0].l_index = 0;
  thread_array[0].r_index = num - 1;

  start = clock();

  //Then new threda is created for the running quick sort parallel
  ret = pthread_create(&threads[0], &attr, qsort_parallel, (void *)&thread_array[0]);


  //For checking of the error code if comes in the pthread cretaion
  if (ret) {
    printf("ERROR code from thread creation is %d\n", ret);
    exit(-1);
  }


  //Waiting for the individual threads to complete as when the first threda is created it creates the other two threads
  int r;


  //So we are waiting for each of the threads to complete and then our quick sort will finish
  for (int j = 0; j < N; ++j) {
    r = pthread_join(threads[j], &status);
  }

  end = clock();

 //This is the time calculation
  float secs = ((float)end - (float)start) / CLOCKS_PER_SEC;

  //Output the time
  cout << "Time taken by Parallel quick sort = " << secs << " seconds" << endl;

  //Then we will free the list memory where we dynamically store the random numbers which served as input for the parallel quick sort
  free(list);

  //Main thread exit aftre the work is over
  pthread_exit(NULL);
  return 0;

}

