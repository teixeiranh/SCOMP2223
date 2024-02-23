#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_KEYS 8000 // total keys, as specified
#define THREADS 16 // total number of threads, as specified
#define NUMBER_OF_NUMBERS 49 // total number of numbers

int counts[NUMBER_OF_NUMBERS] = {0}; // number of counts that we get in the process
int database_of_keys[TOTAL_KEYS][5]; // matrix to store the database of keys
pthread_mutex_t mtx[NUMBER_OF_NUMBERS]; // 49 mutexes, one for each number 

/*
    Method that each of the threads will perform.
    We will need to pass as argument the thread number.
*/
void *count_values(void *arg)
{
    int start_index = *((int *)arg) * (TOTAL_KEYS / THREADS);
    for (int i = start_index; i < start_index + (TOTAL_KEYS / THREADS); i++)
    {
        for (int j = 0; j < 5; j++)
        {
            int number = database_of_keys[i][j];
            pthread_mutex_lock(&mtx[number]);
            // Critical zone---
            counts[number]++;
            // Critical zone---
            pthread_mutex_unlock(&mtx[number]);
        }
    }
    return NULL;
}

/*
Fill the database of keys.
*/
void fill_database_of_keys(void)
{
    srand(time(NULL));

    for (int i = 0; i < TOTAL_KEYS; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            database_of_keys[i][j] = rand() % NUMBER_OF_NUMBERS;
            // printf("%d%d=%d",i,j,database_of_keys[i][j]);
        }
    }
    // printf("First line is:\t%d\t%d\t%d\t%d\t%d\n",
    //        database_of_keys[0][0],
    //        database_of_keys[0][1],
    //        database_of_keys[0][2],
    //        database_of_keys[0][3],
    //        database_of_keys[0][4]);
    // printf("Last line is:\t%d\t%d\t%d\t%d\t%d\n",
    //        database_of_keys[7999][0],
    //        database_of_keys[7999][1],
    //        database_of_keys[7999][2],
    //        database_of_keys[7999][3],
    //        database_of_keys[7999][4]);
}

/*
    Main function.

    In this case we are going to use an array of mutexes, like it was suggested.
    Each mutex will protect the counter of a specific value in the shared array.
    In this way we avoid any kind of race condition, making this solution work
    properly.
*/
int main(int args, char *argv[])
{
    fill_database_of_keys();

    // Here we are going to initialize all of the 49 mutexes
    for (int i = 0; i < NUMBER_OF_NUMBERS; i++)
    {
        pthread_mutex_init(&mtx[i], NULL);
    }

    // Create the threads
    pthread_t threads[THREADS];
    int threads_arguments[THREADS];

    // Let threads do the work
    for (int j = 0; j < THREADS; j++)
    {
        threads_arguments[j] = j;
        pthread_create(&threads[j], NULL, count_values, &threads_arguments[j]);
    }

    // Join threads
    for (int k = 0; k < THREADS; k++)
    {
        pthread_join(threads[k], NULL);
    }

    // Print the list of numbers
    for (int i = 0; i < NUMBER_OF_NUMBERS; i++)
    {
        printf("Number %d: %d times\n", i + 1, counts[i]);
    }

    // Destroy all the created mutexes
    for (int i = 0; i < NUMBER_OF_NUMBERS; i++)
    {
        pthread_mutex_destroy(&mtx[i]);
    }
    
    return 0;
}