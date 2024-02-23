#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 50000
#define NUM_CHILDREN 10
#define ELEMENTS_PER_CHILD (ARRAY_SIZE / NUM_CHILDREN)

/*
Typedef structure for the exercise:
    - int customer_code - customer code;
    - int product_code - product code;
    - quantity - amount of sales.
*/
typedef struct
{
    int customer_code;
    int product_code;
    int quantity;
} SaleRecord;

/* main() function solving the exercise. */
int main()
{
    int i, j;
    int products[ARRAY_SIZE] = {0}; // initialize array to 0, to avoid garbage
    int pipes[NUM_CHILDREN][2];

    srand(time(NULL)); // used time because it will always give a different seed

    pid_t pids[NUM_CHILDREN] = {0};
    // create child processes
    for (i = 0; i < NUM_CHILDREN; i++)
    {
        // create pipe
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe error!");
            exit(EXIT_FAILURE);
        }

        // fork a child
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork error!");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) // child process
        {
            close(pipes[i][0]); // Close the read end of the pipe

            int start_index = i * ELEMENTS_PER_CHILD;
            int end_index = start_index + ELEMENTS_PER_CHILD;
            for (j = start_index; j < end_index; j++)
            {
                SaleRecord record = {
                    .customer_code = rand() % 100,
                    .product_code = rand() % 100,
                    .quantity = rand() % 100};
                // fill sale record with random values
                if (record.quantity > 20)
                {
                    // send product code to parent process
                    write(pipes[i][1], &record.product_code, sizeof(int));
                }
            }
            close(pipes[i][1]); // close the write end of the pipe
            exit(0);            // terminate child process
        }
        else
        {
            close(pipes[i][1]); // Close the write end of the pipe
        }
    }

    // Wait for all child processes to finish and read their results
    for (i = 0; i < NUM_CHILDREN; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) // evaluate if child proceeded normally
        {
            // Read the results sent by this child process
            int start = i * ELEMENTS_PER_CHILD;
            int end = start + ELEMENTS_PER_CHILD;
            for (int j = start; j < end; j++)
            {
                int value;
                read(pipes[i][0], &value, sizeof(value));
                products[j] = value;
            }
        }

        close(pipes[i][0]); // Close the read end of the pipe
    }

    //TODO: needs refactor! Not working properly!
    // print product codes found by child processes
    int count = 0;
    printf("Products sold more than 20 units:\n");
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        if (products[i] > 20)
        {
            printf("%d\n", products[i]);
            count++;
        }
    }
    printf("%d products found!\n");

    return 0;
}
