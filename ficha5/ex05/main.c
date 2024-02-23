#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define ARRAY_SIZE 1000
#define THREADS 3

int lowest_balance = 0;
int highest_balance = 0;
double average_balance = 0;

void *find_lowest(void *arg)
{
    int *array = (int *)arg;
    int lowest = array[0];

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (array[i] < lowest)
        {
            lowest_balance = array[i];
        }
    }

    pthread_exit(NULL);
}

void *find_greatest(void *arg)
{
    int *array = (int *)arg;
    int greatest = array[0];

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if (array[i] > greatest)
        {
            highest_balance = array[i];
        }
    }

    pthread_exit(NULL);
}

void *compute_average(void *arg)
{
    int *array = (int *)arg;
    long sum = 0;

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        sum += array[i];
    }

    average_balance = (double)sum / ARRAY_SIZE;

    pthread_exit(NULL);
}

/*
    Main function.
    This function is working properly for positive numbers only.
*/
int main(int args, char *argv[])
{
    pthread_t thread[THREADS];
    // thread_data_t thread_data[THREADS];
    int array_to_be_searched[ARRAY_SIZE];

    // Filling the array with noduplicate value, in this case 1-1000.
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array_to_be_searched[i] = i;
    }

    pthread_create(&thread[0], NULL, find_lowest, &array_to_be_searched);
    pthread_create(&thread[1], NULL, find_greatest, &array_to_be_searched);
    pthread_create(&thread[2], NULL, compute_average, &array_to_be_searched);

    for (int i = 0; i < THREADS; i++)
    {
        // void *result;
        pthread_join(thread[i], NULL);
    }

    // Print the values pretended.
    printf("Lowest value is: %d.\n", lowest_balance);
    printf("Highest value is: %d.\n", highest_balance);
    printf("Average value is: %f.\n", average_balance);

    return 0;
}