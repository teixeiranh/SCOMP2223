#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int numberOfProcesses = 10;
    pid_t pids[10] = {0}; // initialize array to avoid garbage values

    for (int i = 0; i < numberOfProcesses; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("fork failed!!!");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) // runs only if it is a child
        {
            int value = 0;
            printf("---\n");
            for (int j = 0; j < 100; j++)
            {
                value = j + i * 100 + 1; // example: j=50, i=8 -> value=851

                printf("%d \n", value);
            }
            printf("---\n");
            exit(i);
        }
        else // parent will wait for all the children
        {
            int status = 0;
            for (int i = 0; i < numberOfProcesses; i++)
            {
                waitpid(pids[i], &status, 0);
            }
        }
    }

    return 0;
};

/*
a)
    Yes the output is sorted.
b)
    Yes I can guarantee it will always be sorted.
    In this case, we will only have 2 active processes, because we always set
    the father to wait in case it was found by the CPU first (line 28).
    When we find a child (line 15) we print the numbers and then we terminate the
    process, having ordered control on the created processes and the numbers printed.

*/
