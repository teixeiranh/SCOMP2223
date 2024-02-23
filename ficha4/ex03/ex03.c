#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

#define SHM_NAME "/my_shared_memory" // Shared memory object name
#define SEM_NAME "/my_semaphore"     // Semaphore object name
#define MAX_STRINGS 50               // Maximum number of strings
#define MAX_STRING_LEN 80            // Maximum length of each string
#define MAX_WAIT_TIME 12             // Maximum wait time for acquiring semaphore

int main()
{
    srand(time(NULL)); // Seeding random number generator

    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT, 0644, 1); // Open or create the semaphore
    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    int sharedMemoryFiledes = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); // Open or create the shared memory object
    if (sharedMemoryFiledes == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(sharedMemoryFiledes, MAX_STRINGS * MAX_STRING_LEN) == -1) // Set the size of the shared memory object
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    char(*shared_memory)[MAX_STRING_LEN] = mmap(NULL, MAX_STRINGS * MAX_STRING_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFiledes, 0); // Map the shared memory object into the process's address space
    if (shared_memory == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    int currentStringIndex = 0;

    while (currentStringIndex < MAX_STRINGS)
    {
        time_t start = time(NULL);
        while (sem_trywait(semaphore) == -1) // Try to acquire the semaphore
        {
            if (time(NULL) - start >= MAX_WAIT_TIME) // Check if the waiting time exceeds the maximum wait time
            {
                fprintf(stderr, "Timeout while waiting for semaphore\n");
                exit(EXIT_FAILURE);
            }
            sleep(1); // Sleep for a second before trying again
        }

        // Proceed only when the semaphore is acquired
        if (rand() % 100 < 70 && currentStringIndex < MAX_STRINGS) // 70% probability to write a new string
        {
            snprintf(shared_memory[currentStringIndex], MAX_STRING_LEN, "I'm the Father - with PID %d", getpid()); // Write the string to the shared memory
            ++currentStringIndex;
        }
        else if (currentStringIndex > 0) // 30% probability to remove the last string
        {
            --currentStringIndex;
            memset(shared_memory[currentStringIndex], 0, MAX_STRING_LEN); // Clear the last string from the shared memory
        }
        sleep(rand() % 5 + 1); // Sleep outside the semaphore lock
        sem_post(semaphore);   // Release the semaphore
    }

    if (sem_close(semaphore) == -1) // Close the semaphore
    {
        perror("sem_close problem, ma men");
        exit(EXIT_FAILURE);
    }

    if (munmap(shared_memory, MAX_STRINGS * MAX_STRING_LEN) == -1) // Unmap the shared memory object from the process's address space
    {
        perror("munmap cause a critical error. Throw you pc over the window");
        exit(EXIT_FAILURE);
    }

    if (close(sharedMemoryFiledes) == -1) // Close the shared memory object
    {
        perror("close did not close properly :/");
        exit(EXIT_FAILURE);
    }

    return 0;
}
