#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MATRIX_DIM 8
#define MATRICES 2
#define THREADS_TO_FILL 2
#define THREADS_TO_CALCULATE 1

int first_matrix[MATRIX_DIM][MATRIX_DIM];
int second_matrix[MATRIX_DIM][MATRIX_DIM];
int calculate_matrix[MATRIX_DIM][MATRIX_DIM];

/*
    Function to fill the first matrix.
*/
void *fill_first_matrix(void *arg)
{
    int value = *((int *)arg); // cast the generic value

    for (int i = 0; i < MATRIX_DIM; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            first_matrix[i][j] = value;
        }
    }
    pthread_exit(NULL); // used 0 here so it shows it is successful
}

/*
    Function to fill the second matrix.
*/
void *fill_second_matrix(void *arg)
{
    int value = *((int *)arg); // cast the generic value

    for (int i = 0; i < MATRIX_DIM; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            second_matrix[i][j] = value;
        }
    }
    pthread_exit(NULL); // used 0 here so it shows it is successful
}

/*
    Function to perform calculations.
*/
void *calculate_product(void *arg)
{
    int line_to_calculate = *((int *)arg); // cast the generic value
    int i, j, k;

    // First we are going to set the values for the resulting matrix to 0.
    for (i = 0; i < MATRIX_DIM; i++)
    {
        for (j = 0; j < MATRIX_DIM; j++)
        {
            calculate_matrix[i][j] = 0;
        }
    }

    // Perform matrix multiplication
    for (i = 0; i < MATRIX_DIM; i++)
    {
        for (j = 0; j < MATRIX_DIM; j++)
        {
            for (k = 0; k < MATRIX_DIM; k++)
            {
                calculate_matrix[i][j] = +first_matrix[i][k] * second_matrix[k][j];
            }
        }
    }
    pthread_exit(NULL); // used 0 here so it shows it is successful
}

/*
    Main function.
*/
int main(int args, char *argv[])
{
    pthread_t thread_to_fill[THREADS_TO_FILL];
    pthread_t thread_to_calculate[THREADS_TO_CALCULATE]; // array of "thread_to_calculate"
    // int line_to_calculate[THREADS_TO_CALCULATE] = {0, 1, 2, 3, 4, 5, 6, 7};
    // Value used to fill the matrices. Just to make this simpler.
    int value_first_matrix = 1;
    int value_second_matrix = 2;

    // Fill matrix one and matrix two.
    pthread_create(&thread_to_fill[0], NULL, fill_first_matrix,NULL);
    pthread_create(&thread_to_fill[1], NULL, fill_second_matrix, NULL);

    pthread_join(thread_to_fill[0], NULL);
    pthread_join(thread_to_fill[1], NULL);

    pthread_create(&thread_to_calculate[0], NULL, calculate_product, NULL);
    pthread_join(thread_to_calculate[0], NULL);

    for (int i = 0; i < MATRIX_DIM; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            printf(" %d", calculate_matrix[i][j]);
        }
        printf("\n");
        
    }

    return 0;
}