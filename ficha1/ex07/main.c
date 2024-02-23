#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000

int main()
{
    int numbers[ARRAY_SIZE]; /* array to lookup */
    int n;                   /* the number to find */
    time_t t;                /* needed to init. the random number generator (RNG)*/

    int i;
    /* initializes RNG(srand(): stdlib.h;time(): time.h) */
    srand((unsigned)time(&t));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        numbers[i] = rand() % 10000;
    }

    /* initialize n */
    n = rand() % 10000;

    // Solution-----------------------------------------------------------------
    pid_t pid;
    int status;
    int counterFather = 0, totalCount = 0;

    pid = fork();

    /* check if the process was properly created */
    if (pid < 0)
    {
        printf("Error creating the processes!\n");
        exit(1);
    }

    if (pid == 0) // child code
    {
        int counterChild = 0;                       // created here to limit scope
        for (int ii = 0; ii < ARRAY_SIZE / 2; ii++)
        {
            if (numbers[ii] == n)
            {
                counterChild++;
            }
        }
        exit(counterChild); // it is here that we pass the value found by the child
    }
    else // parent code
    {
        for (int jj = ARRAY_SIZE; jj >= ARRAY_SIZE / 2; jj--)
        {
            if (numbers[jj] == n)
            {
                counterFather++;
            }
        }
    }
    
    /* here we will send the return value from the child to the status variable (line 67),
    then we assign its value to the variable totalCount using the function WEXITSTATUS.*/
    wait(&status);                    // wait for the child to finish
    totalCount = WEXITSTATUS(status); // get the child value
    totalCount += counterFather;      // add the occurrences found by the parent process
    printf("Number %d was found %d times.\n", n, totalCount);

    return 0;
}
