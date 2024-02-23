#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

/* Function to convert the input time to microseconds. */
long time_to_microseconds(char timeType, int timeValue)
{
    long timeMicro; // Time in microseconds

    // Switch case to convert the time type
    switch (timeType)
    {
    case 'S': // seconds
        timeMicro = timeValue * 1000000;
        break;
    case 's': // microseconds
        timeMicro = timeValue;
        break;
    default:
        printf("Wrong time type ladies and gentlemen. Use 'S' for seconds and 's' for microseconds.\n");
        exit(EXIT_FAILURE);
    }

    return timeMicro;
}

/* Method to create child process. */
void child_process(sem_t *semaphore, long executionTimeMicroseconds)
{
    usleep(executionTimeMicroseconds); // sleep for the specified time
    sem_post(semaphore);
}

/*
    Main function to solve exercise 09 from PL4 on Semaphores.

    We will only use 1 semaphore with the help of time functions to properly measure the time it takes
    to run a process.
*/
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Failed to read documentation once again??? \n\
        Aren't you ashamed of treating intellectual property this way? \n\
        Here is a tip for you, next time try: \"testpar <n> <time>\"\n\
        (n - number of processes, time - time for each to run)\n");
        return 1;
    }

    int numberOfChildProcesses = atoi(argv[1]);         // get integer part
    char *timeString = argv[2];                         // get  string part
    char timeType = timeString[strlen(timeString) - 1]; // last character
    int timeValue = atoi(timeString);                   // get integer part

    long executionTimeMicroseconds = time_to_microseconds(timeType, timeValue);

    sem_t *semaphore = sem_open("sem", O_CREAT, 0600, 0);
    if (semaphore == SEM_FAILED)
    {
        printf("Error creating the semaphores.\n");
        exit(EXIT_FAILURE);
    }

    struct timeval start, end; // struct from time.h
    long elapsedTimeMicroseconds;

    gettimeofday(&start, NULL); // Register the start time

    for (int i = 0; i < numberOfChildProcesses; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork error!");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) // child process
        {
            child_process(semaphore, executionTimeMicroseconds);
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < numberOfChildProcesses; i++)
    {
        sem_wait(semaphore);
    }

    gettimeofday(&end, NULL); // Register the end time

    elapsedTimeMicroseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Elapsed time: %ld microseconds\n", elapsedTimeMicroseconds);

    // Clean/close the semaphores
    if (sem_close(semaphore) == -1)
    {
        perror("sem_close error! :/");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink("sem") == -1)
    {
        perror("sem_unlink error! :/");
        exit(EXIT_FAILURE);
    }

    return 0;
}
