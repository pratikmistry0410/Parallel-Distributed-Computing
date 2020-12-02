#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define DIM 10
#define RMAX 1000000

int thread_count;
int swapped = 0;
int n;
int *a;

void v_initiate() {
	int i;
	srand(time(NULL));
	for(i = 0; i < DIM; i++)
		a[i] = rand() % RMAX;

	printf("Vetor Original\n");
	for(i = 0; i < DIM; i++)
		printf("%d ", a[i]);
	printf("\n");
}

// Function to use random number generator to generate a list of integers from values between 0 to RMAX defined above.
void generate_list(int list[], int n) {
    int i;
    srandom(1);
    for (i = 0; i < n; i++)
        list[i] = random() % RMAX;
}

void bubble(int i) {
	int tmp, mod;
	
	mod = i % 2;

	if (i != n-1) {
		if(a[i] > a[i+1]) {	
			tmp = a[i];
			a[i] = a[i+1];
			a[i+1] = tmp;
			swapped = 1;
		}
	}
}

int main(int argc, char* argv[]) {
	long i, par_impar = 0;
	pthread_t* thread_handles;

    if (argc!=3){
		fprintf(stderr, "Please follow this format: ./bubblesort_pthread <thread count> <N> [o]\n");
        fprintf(stderr, "N = Number of elements in list\n");
        fprintf(stderr, "where N should be evenly divisible by thread count..!!\n");
        fprintf(stderr, "'o': If you want to display original and sorted lists\n");
        exit(0);       // Terminate the program
	}

    thread_count = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);

    a = malloc(n*sizeof(int));
    thread_handles = malloc (thread_count*sizeof(pthread_t));

    // v_initiate();
    generate_list(a, n);

    printf("Original list is: \n");
	for(i = 0; i < n; i++)
		printf("%d, ", a[i]);
	printf("\n");
		
	do {

		swapped = 0;
		
		for(i = 0; i < thread_count; i+=2)
			pthread_create(&thread_handles[i], NULL, &bubble, i);
		for(i = 0; i < thread_count; i+=2)
			pthread_join(thread_handles[i], NULL);

		swapped = 0;

		for(i = 0; i < thread_count; i+=2)
			pthread_create(&thread_handles[i], NULL, &bubble, i);
		for(i = 0; i < thread_count; i+=2)
			pthread_join(thread_handles[i], NULL);

	} while(swapped == 1);
	
	printf("The sorted list using bubble sort is: \n");
	for(i = 0; i < n; i++)
		printf("%d, ", a[i]);
	printf("\n");

	exit(0);
}