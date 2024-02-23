#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

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
        sem_wait(mutex);            // Acquire mutex semaphore for mutual exclusion
        shm->reader_count++;        // Increment reader count in shared memory
        if (shm->reader_count == 1) // If this is the first reader, acquire write semaphore to block writers
        {
            sem_wait(wrt);
        }
        sem_post(mutex); // Release mutex semaphore

        sleep(1); // Simulate some processing time
        printf("Data read from memory: %s\n", shm->data);
        printf("Number of readers: %d\n", shm->reader_count);

        sem_wait(mutex);            // Acquire mutex semaphore
        shm->reader_count--;        // Decrement reader count in shared memory
        if (shm->reader_count == 0) // If no more readers, release write semaphore to allow writers
        {
            sem_post(wrt);
        }
        sem_post(mutex); // Release mutex semaphore
    }

    return 0; // Exit the program
}
