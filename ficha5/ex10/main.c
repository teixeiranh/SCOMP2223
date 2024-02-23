#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VEC_SIZE 10000
#define NUMBER_OF_THREADS 6 // total number of threads, as specified
#define NUMBER_OF_HYPERMARKETS 3
#define NUMBER_OF_PRODUCTS 5
#define NUMBER_OF_FILTERING_THREADS 3
#define NUMBER_OF_COMPUTING_THREADS 3

typedef struct
{
    int id_h; // Hypermarket id
    int id_p; // Product id
    double p; // Price of the product
} HypermarketData;

typedef struct
{
    int id_h;           // Hypermarket id
    double lowest_cost; // Lowest cost found
} AnalysisResults;

HypermarketData vec[VEC_SIZE] = {0}; // Hypermarket database
HypermarketData *hm1 = NULL;         // Results for hypermarket 1
int hm1_size = 0;
HypermarketData *hm2 = NULL; // Results for Hypermarket 2
int hm2_size = 0;
HypermarketData *hm3 = NULL; // Results for Hypermarket 3
int hm3_size = 0;

pthread_mutex_t filtering_mutex;
pthread_mutex_t computing_mutex;
pthread_cond_t filtering_condition;

int filtering_thread_counter = 0;

AnalysisResults analysisResults;

/*
    Method that each of the threads will perform.
*/
void *filtering_threads(void *arg)
{
    int array_index = *((int *)arg);

    for (int i = 0; i < VEC_SIZE; i++)
    {
        if (vec[i].id_h == 1 && array_index == 0) // Hypermarket 1
        {
            hm1_size++;
            hm1 = (HypermarketData *)realloc(hm1, hm1_size * sizeof(HypermarketData));
            hm1[hm1_size - 1] = vec[i];
        }
        else if (vec[i].id_h == 2 && array_index == 1) // Hypermarket 2
        {
            hm2_size++;
            hm2 = (HypermarketData *)realloc(hm2, hm2_size * sizeof(HypermarketData));
            hm2[hm2_size - 1] = vec[i];
        }
        else if (vec[i].id_h == 3 && array_index == 2) // Hypermarket 3
        {
            hm3_size++;
            hm3 = (HypermarketData *)realloc(hm3, hm3_size * sizeof(HypermarketData));
            hm3[hm3_size - 1] = vec[i];
        }
    }

    pthread_mutex_lock(&filtering_mutex);

    filtering_thread_counter++;

    // Signal T4, T5 and T6 that they can proceed
    if (filtering_thread_counter == NUMBER_OF_FILTERING_THREADS)
    {
        pthread_cond_broadcast(&filtering_condition);
    }

    pthread_mutex_unlock(&filtering_mutex);

    return NULL;
}

/*
    Method that each of the threads will perform.
*/
void *computing_threads(void *arg)
{

    pthread_mutex_lock(&filtering_mutex);
    while (filtering_thread_counter < NUMBER_OF_FILTERING_THREADS)
    {
        pthread_cond_wait(&filtering_condition, &filtering_mutex); // wait until every other thread has already calculated
    }
    pthread_mutex_unlock(&filtering_mutex);

    int thread_index = *((int *)arg);
    int sum = 0;
    double average = 0;

    if (thread_index == 0) // Hypermarket 1
    {
        for (int i = 0; i < hm1_size; i++)
        {
            sum += hm1[i].id_p;
        }
        average = (double)sum / hm1_size;
        printf("thread 4, sum=%d\n", sum);
        printf("thread 4, average=%f\n", average);
    }
    else if (thread_index == 1) // Hypermarket 2
    {
        for (int i = 0; i < hm2_size; i++)
        {
            sum += hm2[i].id_p;
        }
        average = (double)sum / hm2_size;
        printf("thread 5, sum=%d\n", sum);
        printf("thread 5, average=%f\n", average);
    }
    else if (thread_index == 2) // Hypermarket 3
    {
        for (int i = 0; i < hm3_size; i++)
        {
            sum += hm3[i].id_p;
        }
        average = (double)sum / hm3_size;
        printf("thread 6, sum=%d\n", sum);
        printf("thread 6, average=%f\n", average);
    }

    pthread_mutex_lock(&computing_mutex);
    if (analysisResults.lowest_cost == 0)
    {
        analysisResults.lowest_cost = average;
        analysisResults.id_h = thread_index;
    }
    else if (analysisResults.lowest_cost > average)
    {
        analysisResults.lowest_cost = average;
        analysisResults.id_h = thread_index;
    }

    pthread_mutex_unlock(&computing_mutex);

    return NULL;
}

/*
    Fill the data array.
*/
void fill_data_array(void)
{
    srand(time(NULL));

    for (int i = 0; i < VEC_SIZE; i++)
    {
        vec[i].id_h = rand() % NUMBER_OF_HYPERMARKETS + 1;
        vec[i].id_p = rand() % NUMBER_OF_PRODUCTS + 1;
        vec[i].p = (double)(rand() % 100 + 1) / 10;
    }
}

/*
    Main function.
*/
int main(int args, char *argv[])
{
    int threads_arguments[NUMBER_OF_FILTERING_THREADS];
    fill_data_array(); // fills the random array of numbers

    pthread_t threads[NUMBER_OF_THREADS];

    pthread_mutex_init(&filtering_mutex, NULL);
    pthread_cond_init(&filtering_condition, NULL);

    for (int i = 0; i < NUMBER_OF_FILTERING_THREADS; i++)
    {
        threads_arguments[i] = i;
        pthread_create(&threads[i], NULL, filtering_threads, &threads_arguments[i]);
    }

    for (int i = 0; i < NUMBER_OF_COMPUTING_THREADS; i++)
    {
        threads_arguments[i] = i;
        pthread_create(&threads[i + 3], NULL, computing_threads, &threads_arguments[i]);
    }

    for (int i = 0; i < NUMBER_OF_FILTERING_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Join threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("The final results are: HM_id: %d and prince=%f.\n ", analysisResults.id_h, analysisResults.lowest_cost);

    return 0;
}