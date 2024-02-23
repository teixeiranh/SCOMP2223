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
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);          // Create shared memory segment
    shm = (struct shared_memory *)shmat(shmid, (void *)0, 0); // Attach shared memory segment

    mutex = sem_open("/mutex", O_CREAT, 0644, 1); // Create/open mutex semaphore
    if (mutex == SEM_FAILED)
    {
        perror("sem_open error for mutex");
        exit(EXIT_FAILURE);
    }

    wrt = sem_open("/write", O_CREAT, 0644, 1); // Create/open write semaphore
    if (wrt == SEM_FAILED)
    {
        perror("sem_open error for write");
        exit(EXIT_FAILURE);
    }

    shm->reader_count = 0; // Initialize reader count in shared memory
    shm->writer_count = 0; // Initialize writer count in shared memory
    strcpy(shm->data, ""); // Initialize data in shared memory

    shmdt(shm); // Detach shared memory segment

    return 0; // Exit the program
}
