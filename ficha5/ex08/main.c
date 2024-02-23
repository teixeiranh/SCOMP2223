#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000     // total keys, as specified
#define NUMBER_OF_THREADS 5 // total number of threads, as specified

int data_array[ARRAY_SIZE] = {0};   // number of counts that we get in the process
int result_array[ARRAY_SIZE] = {0}; // matrix to store the database of keys
int calculated_counter = 0;         // counter of calculated threads

pthread_mutex_t mutex;
pthread_cond_t condition;

/*
    Method that each of the threads will perform.
*/
void *perform_calculations(void *arg)
{
    // Perform calculations, all threads parallel
    int start_index = *((int *)arg) * (ARRAY_SIZE / NUMBER_OF_THREADS);
    for (int i = start_index; i < start_index + (ARRAY_SIZE / NUMBER_OF_THREADS); i++)
    {
        result_array[i] = data_array[i] * 10 + 2;
    }

    // Increment the counter of calculated threads and signal if it is last thread to perform calculation
    pthread_mutex_lock(&mutex);
    calculated_counter++;
    if (calculated_counter == NUMBER_OF_THREADS)
    {
        pthread_cond_broadcast(&condition); // it will broadcast a message to all other threads waiting...
    }
    pthread_mutex_unlock(&mutex);

    // Verify if all threads already calculated, if not wait...
    pthread_mutex_lock(&mutex);
    while (calculated_counter < NUMBER_OF_THREADS)
    {
        pthread_cond_wait(&condition, &mutex); // wait until every other thread has already calculated
    }

    // Print all values
    for (int i = start_index; i < start_index + (ARRAY_SIZE / NUMBER_OF_THREADS); i++)
    {
        printf("Element %d=%d\n", i, result_array[i]);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

/*
    Fill the data array.
*/
void fill_data_array(void)
{
    srand(time(NULL));

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        data_array[i] = rand() % 100 + 1; // random number from 1 to 100
    }
}

/*
    Main function.
*/
int main(int args, char *argv[])
{
    pthread_t threads[NUMBER_OF_THREADS]; // creates array of 5 threads
    int threads_arguments[NUMBER_OF_THREADS];
    pthread_mutex_init(&mutex, NULL);    // initialize mutex
    pthread_cond_init(&condition, NULL); // initialize condition variable

    fill_data_array(); // fills the random array of numbers

    // Create 5 threads.
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        threads_arguments[i] = i;
        pthread_create(&threads[i], NULL, perform_calculations, &threads_arguments[i]);
    }

    // Join threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}