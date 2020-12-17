/* Bitonic sort of a list of integers using Pthreads
 *
 * Compile:  gcc -g -Wall -o bitonic_pthreads bitonic_pthreads.c -lpthread
 * Run:      ./bitonic_pthreads <thread count> <N> 
 *           N = number of ints in the list 
 *           If 'o' is included on the command line, the program
 *           will print the original list and the sorted list
 * 
 * Output:   If 'o' is included on the command line, the original
 *           list and the sorted list will be printed.  
 *           The elapsed time for the sort will always be printed.
 *
 * Notes:
 * 1.  thread_count should be a power of 2 for bitonic search implemented using pthreads
 * 2.  N = list_size should be evenly divisible by thread_count
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define RMAX 1000000                                     // Random values in the range 0 to RMAX-1

int thread_count;
int bar_count = 0;
pthread_mutex_t bar_mutex;
pthread_cond_t bar_cond;
int n;
int *array_1, *array_2;
int *l_a, *l_b;


// Utility functions definition to assist the program execution.
void print_error_message(char* program);
void get_user_input(int argc, char *argv[], int* gen_list_p, int* output_list_p);
void generate_list(int list[], int n);
void read_list_from_console(char prompt[], int list[], int n);
void print_list(char title[], int list[], int n);
void *bitonic_sort(void* rank);
void bitonic_increasing_order_sort(int th_count, int dim, int my_first, int local_n,
      int my_rank);
void bitonic_decreasing_order_sort(int th_count, int dim, int my_first, int local_n,
      int my_rank);
void merge_split_lower_half(int my_rank, int my_first, int local_n,
      int partner);
void merge_split_upper_half(int my_rank, int my_first, int local_n,
      int partner);
int  comparator(const void* x_p, const void* y_p);
void barrier(void);

// Main Function  - Drive Function
int main(int argc, char* argv[]) {
   long thread;
   pthread_t* thread_handles; 
   int gen_list, output_list;

   get_user_input(argc, argv, &gen_list, &output_list);

   thread_handles = malloc (thread_count*sizeof(pthread_t));
   pthread_mutex_init(&bar_mutex, NULL);
   pthread_cond_init(&bar_cond, NULL);
   array_1 = malloc(n*sizeof(int));
   array_2 = malloc(n*sizeof(int));
   l_a = array_1;
   l_b = array_2;

   if (1)
      generate_list(array_1, n);
   else
      read_list_from_console("Enter the list: ", array_1, n);
   if (output_list)
      print_list("The input list is: ", array_1, n);

   clock_t start2, end2;
   start2 = clock();
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL, bitonic_sort, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL);
 
   end2 = clock();

   float seconds = ((float)end2 - (float)start2) / CLOCKS_PER_SEC;
   printf("Total time taken to sort the list = %e seconds\n", seconds);
   if (output_list)
      print_list("The sorted list is: ", l_a, n);

   free(array_1);
   free(array_2);

   pthread_mutex_destroy(&bar_mutex);
   pthread_cond_destroy(&bar_cond);
   free(thread_handles);
   return 0;
} 


// Barrier to block all threads until all threads have called barrier
void barrier(void) {
   pthread_mutex_lock(&bar_mutex);
   bar_count++;
   if (bar_count == thread_count) {
      bar_count = 0;
      pthread_cond_broadcast(&bar_cond);
   } else {
      while (pthread_cond_wait(&bar_cond, &bar_mutex) != 0);
   }
   pthread_mutex_unlock(&bar_mutex);
}


// Function to print error message if user passes bad argument
void print_error_message(char* program) {
   fprintf(stderr, "Please follow this format: %s <thread count> <N> [o]\n", program);
   fprintf(stderr, "N = Number of elements in list\n");
   fprintf(stderr, "where N should be evenly divisible by thread count..!!\n");
   fprintf(stderr, "'o': If you want to display original and sorted lists\n");
   exit(0);       // Terminate the program
}

// Function to read user input from command line args - Thread count and number of elements
void get_user_input(int argc, char *argv[], int* gen_list_p, int* output_list_p) {
   char c1;

   if (argc < 3 || argc > 5) print_error_message(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   n = strtol(argv[2], NULL, 10);
   if (n % thread_count != 0) print_error_message(argv[0]);

   *gen_list_p = *output_list_p = 0;

   if (argc == 4) {
      c1 = argv[3][0];
      if (c1 == 'g') 
         *gen_list_p = 1;
      else
         *output_list_p = 1;
   } else if (argc == 5) {
      *gen_list_p = 1;
      *output_list_p = 1;
   }
}


// Function to use random number generator to generate a list of integers from values between 0 to RMAX defined above.
void generate_list(int list[], int n) {
   int i;
   srandom(1);
   for (i = 0; i < n; i++)
      list[i] = random() % RMAX;
}


// Function to read a list of integers from console
void read_list_from_console(char prompt[], int list[], int n) {
   int i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++)
      scanf("%d", &list[i]);
}


// Function to print the list in console
void print_list(char message[], int list[], int n) {
   int i;

   printf("%s:\n", message);
   for (i = 0; i < n; i++)
      printf("%d, ", list[i]);
   printf("\n");
}


// Function to compare two ints and determine their relative sizes
int comparator(const void* x_p, const void* y_p) {
   int x = *((int*)x_p);
   int y = *((int*)y_p);

   if (x < y)
      return -1;
   else if (x == y)
      return 0;
   else
      return 1;
}


// Bitonic sort function to sort list of randomly generated integers
void *bitonic_sort(void* rank) {
   long tmp = (long) rank;
   int my_rank = (int) tmp; 
   int local_n = n/thread_count;
   int my_first = my_rank*local_n;
   unsigned th_count, and_bit, dim;

   /* Sort each sublist */
   qsort(array_1 + my_first, local_n, sizeof(int), comparator);  
   barrier();
#  ifdef DEBUG
   if (my_rank == 0) print_list("List after qsort", array_1, n);
#  endif
   for (th_count = 2, and_bit = 2, dim = 1; th_count <= thread_count; 
         th_count <<= 1, and_bit <<= 1, dim++) {
      if ((my_rank & and_bit) == 0)
         bitonic_increasing_order_sort(th_count, dim, my_first, local_n, my_rank);
      else
         bitonic_decreasing_order_sort(th_count, dim, my_first, local_n, my_rank);
   }

   return NULL;
} 

//Parallel bitonic sort function to sort a list into increasing order.  This implements a butterfly communication scheme among the threads
void bitonic_increasing_order_sort(int th_count, int dim, int my_first, int local_n,
      int my_rank) {
   int stage;
   int partner;
   int* tmp;
   unsigned eor_bit = 1 << (dim - 1);

   for (stage = 0; stage < dim; stage++) {
      partner = my_rank ^ eor_bit;
      if (my_rank < partner)
         merge_split_lower_half(my_rank, my_first, local_n, partner);
      else
         merge_split_upper_half(my_rank, my_first, local_n, partner);
      eor_bit >>= 1;
      barrier();
      if (my_rank == 0) {
#        ifdef DEBUG
         char title[1000];
#        endif
         tmp = l_a;
         l_a = l_b;
         l_b = tmp;
#        ifdef DEBUG
         sprintf(title, "Th_count = %d, stage = %d", th_count, stage);
         print_list(title, l_a, n);
#        endif
      }
      barrier();
   } 
       
}


//Parallel bitonic sort function to sort a list into decreasing order.  This implements a butterfly communication scheme among the threads
void bitonic_decreasing_order_sort(int th_count, int dim, int my_first, int local_n,
      int my_rank) {
   int stage;
   int partner;
   int* tmp;
   unsigned eor_bit = 1 << (dim - 1);

   for (stage = 0; stage < dim; stage++) {
      partner = my_rank ^ eor_bit;
      if (my_rank > partner)
         merge_split_lower_half(my_rank, my_first, local_n, partner);
      else
         merge_split_upper_half(my_rank, my_first, local_n, partner);
      eor_bit >>= 1;
      barrier();
      if (my_rank == 0) {
#        ifdef DEBUG
         char title[1000];
#        endif
         tmp = l_a;
         l_a = l_b;
         l_b = tmp;
#        ifdef DEBUG
         sprintf(title, "Th_count = %d, stage = %d", th_count, stage);
         print_list(title, l_a, n);
#        endif
      }
      barrier();
   } 
       
}


// Merge two sublists in array l_a keeping lower half in l_b
void merge_split_lower_half(int my_rank, int my_first, int local_n,
      int partner) {
   int ai, bi, xi, i;


   ai = bi = my_first;
   xi = partner*local_n;

#  ifdef DDEBUG
   printf("Th %d > In M_s_lo partner = %d, ai = %d, xi = %d\n",
         my_rank, partner, ai, xi);
#  endif    
   for (i = 0; i < local_n; i++)
      if (l_a[ai] <= l_a[xi]) {
         l_b[bi++] = l_a[ai++];
      } else {
         l_b[bi++] = l_a[xi++];
      }

}


// Function to Merge two sublists in array l_a keeping upper half in l_b
void merge_split_upper_half(int my_rank, int my_first, int local_n,
      int partner) {
   int ai, bi, xi, i;

   ai = bi = my_first + local_n - 1;
   xi = (partner+1)*local_n - 1;

#  ifdef DDEBUG
   printf("Th %d > In M_s_hi partner = %d, ai = %d, xi = %d\n",
         my_rank, partner, ai, xi);
#  endif    

   for (i = 0; i < local_n; i++)
      if (l_a[ai] >= l_a[xi])
         l_b[bi--] = l_a[ai--];
      else
         l_b[bi--] = l_a[xi--];

}