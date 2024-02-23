#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define SHM_SIZE 1024

struct shared_memory
{
    int reader_count;
    int writer_count;
    char data[100];
};

int main()
{
    key_t key;                 // Key for shared memory
    int shmid;                 // Shared memory identifier
    sem_t *mutex;              // Semaphore for mutual exclusion
    sem_t *wrt;                // Semaphore for controlling write access
    struct shared_memory *shm; // Pointer to shared memory structure

    key = ftok("shmfile", 65);                                // Generate key based on file name and identifier
    shmid = shmget(key, SHM_SIZE, 0666);                      // Get existing shared memory segment
    shm = (struct shared_memory *)shmat(shmid, (void *)0, 0); // Attach shared memory segment

    mutex = sem_open("/mutex", O_RDWR); // Open existing mutex semaphore
    if (mutex == SEM_FAILED)
    {
        perror("sem_open error for mutex");
        exit(EXIT_FAILURE);
    }
    
    wrt = sem_open("/write", O_RDWR); // Open existing write semaphore
    if (wrt == SEM_FAILED)
    {
        perror("sem_open error for write");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        sem_wait(mutex);     // Acquire mutex semaphore for mutual exclusion
        shm->writer_count++; // Increment writer count in shared memory
        printf("Number of writers: %d\n", shm->writer_count);
        printf("Number of readers: %d\n", shm->reader_count);
        sem_post(mutex); // Release mutex semaphore

        sem_wait(wrt); // Acquire write semaphore for write access

        sprintf(shm->data, "PID: %d, Time: %ld", getpid(), time(NULL)); // Write data to shared memory

        sem_post(wrt); // Release write semaphore

        sem_wait(mutex);     // Acquire mutex semaphore
        shm->writer_count--; // Decrement writer count in shared memory
        sem_post(mutex);     // Release mutex semaphore
    }

    return 0; // Exit the program
}
