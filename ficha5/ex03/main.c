#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define ARRAY_SIZE 1000
#define THREADS 10
#define SEARCH_RANGE (ARRAY_SIZE / THREADS)

/*
    Structure with the data to be analyzed by each of the threads.
*/
typedef struct
{
    int *array;       // array to be analyzed
    int start_index;  // start index of the search
    int end_index;    // end index of the search
    int search_value; // value to be searched
    int thread_id;    // thread id to be printed
} thread_data_t;

/*
    Function to perform the search within the given array.
*/
void *search_in_array(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    for (int i = data->start_index; i < data->end_index; i++)
    {
        if (data->array[i] == data->search_value)
        {
            printf("Found @ index: %d\n", i);
            // return (void *)&(data->thread_id);
            pthread_exit((void *)&(data->thread_id));
        }
    }

    pthread_exit(NULL);
}

/*
    Main function.
*/
int main(int args, char *argv[])
{
    pthread_t thread[THREADS];
    thread_data_t thread_data[THREADS];
    int array_to_be_searched[ARRAY_SIZE];

    // Filling the array with noduplicate value, in this case 1-1000.
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array_to_be_searched[i] = i;
    }

    int value_to_be_searched = 500; // just to serve as an example

    for (int i = 0; i < THREADS; i++)
    {
        // Fill the data needed to be analysed by each one of the threads
        thread_data[i].array = array_to_be_searched;
        thread_data[i].start_index = i * SEARCH_RANGE;
        thread_data[i].end_index = (i + 1) * SEARCH_RANGE - 1;
        thread_data[i].search_value = value_to_be_searched;
        thread_data[i].thread_id = i + 1;

        // Create the thread
        pthread_create(&thread[i], NULL, search_in_array, &thread_data[i]);
    }

    for (int i = 0; i < THREADS; i++)
    {
        void *result;
        pthread_join(thread[i], &result); // wait for the thread and evaluate it's value

        if (result != NULL)
        {
            int thread_id = *((int *)result);
            printf("Winner is thread id: %d!\n", thread_id);
        }
    }

    return 0;
}