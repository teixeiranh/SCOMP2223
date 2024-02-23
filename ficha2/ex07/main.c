#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILDREN 5
#define TOTAL_NUMBER_OF_ELEMENTS 1000
#define ELEMENTS_PER_CHILD (TOTAL_NUMBER_OF_ELEMENTS / NUM_CHILDREN)
#define MAX_VALUE 1000

int main()
{
    // initialize all the arrays to avoid garbage
    int vec1[TOTAL_NUMBER_OF_ELEMENTS] = {0},
        vec2[TOTAL_NUMBER_OF_ELEMENTS] = {0},
        result[TOTAL_NUMBER_OF_ELEMENTS] = {0};
    int pipes[NUM_CHILDREN][2]; // pipes to store the sum

    // Initialize the input vectors with random values up to MAX_VALUE
    for (int i = 0; i < TOTAL_NUMBER_OF_ELEMENTS; i++)
    {
        vec1[i] = rand() % MAX_VALUE;
        vec2[i] = rand() % MAX_VALUE;
    }

    pid_t pids[NUM_CHILDREN] = {0};
    // Create pipes for each child
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // fork a child
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0) // child process
        {
            close(pipes[i][0]); // Close the read end of the pipe

            // Calculate the sum of the elements in the chunk assigned to this child
            int start = i * ELEMENTS_PER_CHILD;
            int end = start + ELEMENTS_PER_CHILD;
            for (int j = start; j < end; j++)
            {
                int sum = vec1[j] + vec2[j];
                write(pipes[i][1], &sum, sizeof(sum)); // Send the sum to the parent process
            }

            close(pipes[i][1]); // Close the write end of the pipe
            exit(0);            // kills the child process
        }
        else // parent process
        {
            close(pipes[i][1]); // Close the write end of the pipe
        }
    }

    // Wait for all child processes to finish and read their results
    for (int i = 0; i < NUM_CHILDREN; i++)
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
                int sum;
                read(pipes[i][0], &sum, sizeof(sum));
                result[j] = sum;
            }
        }

        close(pipes[i][0]); // Close the read end of the pipe
    }

    // Print the results
    printf("Result array:\n");
    for (int i = 0; i < TOTAL_NUMBER_OF_ELEMENTS; i++)
    {
        printf("%d ", result[i]);
        printf("\n");
    }

    return 0;
}
