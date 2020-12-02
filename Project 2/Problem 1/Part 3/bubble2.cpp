#include <bits/stdc++.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
  
using namespace std; 
#define RMAX 100
// size of array 

int n;  
// maximum number of threads 
int max_threads; 
int a[n];

  
// int a[] = { 2, 1, 4, 9, 5, 3, 6, 10 }; 
int tmp; 
  
// Function to compare and exchange 
// the consecutive elements of the array 
void* compare(void* arg) 
{ 
  
    // Each thread compares 
    // two consecutive elements of the array 
    int index = tmp; 
    tmp = tmp + 2; 
  
    if ((a[index] > a[index + 1]) && (index + 1 < n)) { 
        swap(a[index], a[index + 1]); 
    } 
} 


// Function to use random number generator to generate a list of integers from values between 0 to RMAX defined above.
void generate_list() {
    int i;
    srandom(1);
    for (i = 0; i < n; i++)
        a[i] = random() % RMAX;
}

void oddEven(pthread_t threads[]) 
{ 
    int i, j; 
  
    for (i = 1; i <= n; i++) { 
        // Odd step 
        if (i % 2 == 1) { 
            tmp = 0; 
  
            // Creating threads 
            for (j = 0; j < max_threads; j++) 
                pthread_create(&threads[j], NULL, compare, NULL); 
  
            // joining threads i.e. waiting 
            // for all the threads to complete 
            for (j = 0; j < max_threads; j++) 
                pthread_join(threads[j], NULL); 
        } 
  
        // Even step 
        else { 
            tmp = 1; 
  
            // Creating threads 
            for (j = 0; j < max_threads - 1; j++) 
                pthread_create(&threads[j], NULL, compare, NULL); 
  
            // joining threads i.e. waiting 
            // for all the threads to complete 
            for (j = 0; j < max_threads - 1; j++) 
                pthread_join(threads[j], NULL); 
        } 
    } 
} 
  
// Function to print an array 
void printArray() 
{ 
    int i; 
    for (i = 0; i < n; i++) 
        cout << a[i] << " "; 
    cout << endl; 
} 
  
// Driver Code 
int main() 
{ 
    max_threads = 4;
    n = 100;

    // v_initiate();
    generate_list();
    
    pthread_t threads[max_threads]; 
  
    cout << "Given array is: "; 
    printArray(); 
  
    oddEven(threads); 
  
    cout << "\nSorted array is: "; 
    printArray(); 
  
    return 0; 
} 