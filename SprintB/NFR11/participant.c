#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHARED_MEMORY_NAME "/board_memory"
#define SEMAPHORE_NAME "/board_semaphore"
#define WRITE_LIMIT 10 // Number of writes to perform per participant

int *board;       // Pointer to shared memory
sem_t *semaphore; // Pointer to named semaphore

/*
    Prototype function for NFR11 - Explore Shared Board Synchronization.

    In this case, concurrent participants will increment by one the current value
    in the board. Each participant will write 10 times.
*/
int main(int argc, char *argv[])
{
    int filedes;
    int writeCount = 0;

    // Open shared memory object
    if ((filedes = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("shm_open error");
        exit(1);
    }

    // Set size of shared memory object
    if (ftruncate(filedes, sizeof(int)) == -1)
    {
        perror("ftruncate error");
        exit(1);
    }

    // Map shared memory object into memory
    if ((board = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0)) == MAP_FAILED)
    {
        perror("mmap error");
        exit(1);
    }

    // Initialize board with 0
    *board = 0;

    // Create named semaphore
    if ((semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1)) == SEM_FAILED)
    {
        perror("sem_open eror");
        exit(1);
    }

    while (writeCount < WRITE_LIMIT)
    {
        // Try to get the semaphore
        if (sem_wait(semaphore) < 0)
        {
            perror("sem_wait error");
            break;
        }

        // Increment the board value and write it back
        *board += 1;
        printf("Participant %d wrote %d to the board.\n", getpid(), *board);

        // Release the semaphore
        if (sem_post(semaphore) < 0)
        {
            perror("sem_post error!");
            break;
        }

        // Increment write count
        writeCount++;

        // Sleep a bit before next attempt
        sleep(1);
    }

    return 0;
}
