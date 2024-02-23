#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEMAPHORE_CUSTOMER "/sem_customer" // Semaphore name for the client
#define SEMAPHORE_SELLER "/sem_seller"     // Semaphore name for the seller
#define SHARED_MEM_NAME "/shared_mem"      // Shared memory name
#define NUM_TICKETS 100                    // Total number of tickets

int main()
{
    int *ticketCounter; // Pointer to the shared ticket counter
    int filedes;        // File descriptor for shared memory

    // Open the client semaphore, create it if it doesn't exist, with permissions 0600
    sem_t *semaphoreCustomer = sem_open(SEMAPHORE_CUSTOMER, O_CREAT, 0600, 1);
    if (semaphoreCustomer == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Open the seller semaphore, create it if it doesn't exist, with permissions 0600
    sem_t *semaphoreSeller = sem_open(SEMAPHORE_SELLER, O_CREAT, 0600, 0);
    if (semaphoreSeller == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Create or open the shared memory segment, with read and write permissions
    filedes = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0600);
    if (filedes == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory segment to hold an integer
    if (ftruncate(filedes, sizeof(int)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory segment into the process's address space
    ticketCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, filedes, 0);
    if (ticketCounter == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    *ticketCounter = 1; // Initialize the ticket counter

    // Sell tickets until the ticket counter reaches the maximum number of tickets
    while (*ticketCounter <= NUM_TICKETS)
    {
        sem_wait(semaphoreSeller);                     // Wait for the seller semaphore
        printf("Serving ticket %d\n", *ticketCounter); // Serve the next ticket
        (*ticketCounter)++;                            // Increment the ticket counter
        sem_post(semaphoreCustomer);                   // Signal the client semaphore
        sleep(1);                                      // Simulate the serving time
    }

    // Clean up resources
    if (sem_close(semaphoreCustomer) == -1) // Close the client semaphore
    {
        perror("sem_close error for semaphoreCustomer");
        exit(EXIT_FAILURE);
    }

    if (sem_close(semaphoreSeller) == -1) // Close the seller semaphore
    {
        perror("sem_close error for semaphoreSeller");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEMAPHORE_CUSTOMER) == -1) // Remove the client semaphore
    {
        perror("sem_unlink error for SEMAPHORE_CUSTOMER");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEMAPHORE_SELLER) == -1) // Remove the seller semaphore
    {
        perror("sem_unlink error for SEMAPHORE_SELLER");
        exit(EXIT_FAILURE);
    }

    if (munmap(ticketCounter, sizeof(int)) == -1) // Unmap the shared memory segment
    {
        perror("munmap error");
        exit(EXIT_FAILURE);
    }

    if (close(filedes) == -1) // Close the shared memory segment
    {
        perror("close error");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(SHARED_MEM_NAME) == -1) // Remove the shared memory segment
    {
        perror("shm_unlink error for SHARED_MEM_NAME");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
