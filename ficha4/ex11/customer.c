#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

#define SEMAPHORE_CUSTOMER "/sem_customer" // Semaphore name for the client
#define SEMAPHORE_SELLER "/sem_seller"     // Semaphore name for the seller
#define SHARED_MEM_NAME "/shared_mem"      // Shared memory name

int main()
{
    int *ticketCounter;                                              // Pointer to the shared ticket counter
    int filedes;                                                     // File descriptor for shared memory
    sem_t *semaphoreCustomer = sem_open(SEMAPHORE_CUSTOMER, O_RDWR); // Open the client semaphore for reading and writing
    if (semaphoreCustomer == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    sem_t *semaphoreSeller = sem_open(SEMAPHORE_SELLER, O_RDWR); // Open the seller semaphore for reading and writing
    if (semaphoreSeller == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    filedes = shm_open(SHARED_MEM_NAME, O_RDWR, 0600); // Open the shared memory segment for reading and writing
    if (filedes == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    ticketCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0); // Map the shared memory segment into the process's address space
    if (ticketCounter == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));               // Seed the random number generator with the current time
    int wait_time = rand() % 10 + 1; // Generate a random wait time between 1 and 10 seconds
    sleep(wait_time);                // Sleep for the generated wait time

    sem_post(semaphoreSeller);                                                 // Notify the seller that the client is ready to receive a ticket
    sem_wait(semaphoreCustomer);                                               // Wait for the seller to process the ticket
    printf("Client %d received ticket number %d\n", getpid(), *ticketCounter); // Print the client's process ID and the received ticket number

    // Clean up resources
    if (sem_close(semaphoreCustomer) == -1) // Close the client semaphore
    {
        perror("sem_close error for semaphoreCustomer :/");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphoreSeller) == -1) // Close the seller semaphore
    {
        perror("sem_close error for semaphoreSeller :/");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphoreSeller) == -1) // Close the seller semaphore
    {
        perror("sem_close error for semaphoreSeller :/");
        exit(EXIT_FAILURE);
    }

    if (close(filedes) == -1) // Close the shared memory segment
    {
        perror("close error :/");
        exit(EXIT_FAILURE);
    }
    return 0;
}
