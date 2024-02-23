#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 100

int main()
{
    int numbers[ARRAY_SIZE] = {0}; // array to lookup
    time_t t;                      // needed to init. the random number generator (RNG)

    int i;
    // initializes RNG(srand(): stdlib.h;time(): time.h)
    srand((unsigned)time(&t));

    // initialize array with random numbers (rand(): stdlib.h)
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        numbers[i] = rand() % 255; // 255 because the range is [0,255]
        // printf("value %d\n", numbers[i]);
    }

    // a) ----------------------------------------------------------------------
    pid_t pids[5] = {0};
    int maximums[5] = {0};
    int numbOfProcesses = 5;

    for (int i = 0; i < numbOfProcesses; i++)
    {
        pids[i] = fork();
        int max = 0;

        // verify if child process was created without errors
        if (pids[i] == -1)
        {
            perror("Lord Above, fork failed!!!");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) // runs only if it is a child
        {
            for (int j = i * ARRAY_SIZE / 5; j < i * ARRAY_SIZE / 5 + ARRAY_SIZE / 5; j++)
            {
                if (numbers[j] > max)
                {
                    max = numbers[j];
                    // printf("Value for i%d.j%d is %d \n", i, j, max);
                }
            }
            exit(max); // kills the child process
        }
    }

    // at this point we only have the parent process running!
    int status;
    int maxValue = 0;
    for (int i = 0; i < numbOfProcesses; i++)
    {
        wait(&status);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) > maxValue)
            {
                maxValue = WEXITSTATUS(status);
            }
        }
        else
        {
            printf("London Bridge is falling down!");
            exit(EXIT_FAILURE);
        }
    }

    // b) ----------------------------------------------------------------------
    printf("There you have your maximum, champion: %d!\n", maxValue);

    int result[ARRAY_SIZE] = {0};
    pid_t p = fork();

    // evaluate if any problem during the forking of parent
    if (p == -1)
    {
        perror("fork failed!!!");
        exit(EXIT_FAILURE);
    }

    if (p == 0) // runs only if it is a child
    {
        for (int i = 0; i < ARRAY_SIZE / 2; i++)
        {
            printf("index %d number: %d\n", i, numbers[i]);
            result[i] = ((int)numbers[i] / maxValue) * 100;
            printf("index %d result: %d \n", i, result[i]);
        }
        exit(0);
    }
    // c) ----------------------------------------------------------------------
    else // runs for the parent
    {
        // d) ------------------------------------------------------------------
        sleep(1); // cheap way to try to sort the output
        for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++)
        {
            printf("index %d number: %d\n", i, numbers[i]);
            result[i] = ((int)numbers[i] / maxValue) * 100;
            printf("index %d result: %d \n", i, result[i]);
        }
    }

    waitpid(p, &status, 0); // dezombify

    return (0);
}
