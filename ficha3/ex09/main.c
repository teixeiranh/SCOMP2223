#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Constants to be used in the code */
#define BUFFER_SIZE 10 // When bigger than 10, the producer will wait for space in the buffer
#define NUM_VALUES 30  // Total number of values to exchange between producer and consumer

/* Struct to store the shared memory data */
typedef struct
{
    int numberOfElements;    // Number of elements currently in the buffer
    int readIndex;           // Index for reading
    int writeIndex;          // Index for writing
    int buffer[BUFFER_SIZE]; // Circular buffer for storing integers
} sharedMemory;

/* Consumer method used to use the needed data. */
void consumer_method(sharedMemory *shm)
{
    for (int i = 0; i < NUM_VALUES; ++i)
    {
        while (shm->numberOfElements == 0)
        {
            usleep(1000); // Wait for data in the buffer
        }

        printf("Consumer read value: %d\n", shm->buffer[shm->readIndex]); // Print the value read from the buffer
        shm->readIndex = (shm->readIndex + 1) % BUFFER_SIZE;              // Move read index to next position
        shm->numberOfElements--;                                          // Decrement the number of elements in the buffer
    }
}

/* Producer method used to fill the needed data. */
void producer_method(sharedMemory *shm)
{
    for (int i = 0; i < NUM_VALUES; ++i)
    {
        while (shm->numberOfElements == BUFFER_SIZE)
        {
            usleep(1000); // Wait for data in the buffer
        }

        shm->buffer[shm->writeIndex] = i;                      // Write the current value into the buffer
        shm->writeIndex = (shm->writeIndex + 1) % BUFFER_SIZE; // Move write index to next position
        shm->numberOfElements++;                               // Increment the number of elements in the buffer
    }
}

/* Main function to solve ex09 of PL3. */
int main()
{
    // Constants to facilitate coding
    const int DATA_SIZE = sizeof(sharedMemory);
    const char *sharedMemoryName = "sharedMemory"; // Name of the shared memory object

    int sharedMemoryFiledes = shm_open(sharedMemoryName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // Open a shared memory object
    if (sharedMemoryFiledes == -1)
    {
        perror("shm_open error!");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(sharedMemoryFiledes, sizeof(sharedMemory)) != 0)
    {
        perror("ftruncate error!");
        exit(1);
    }

    // Map the shared memory object into the address space
    sharedMemory *shm = mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFiledes, 0);
    // NULL indicates that the kernel chooses the address at which to map the file
    if (shm == MAP_FAILED)
    {
        perror("mmap error!");
        exit(1);
    }

    memset(shm, 0, DATA_SIZE); // Initialize the shared memory with zeros to avoid garbage values

    pid_t pid = fork(); // Create a new process

    if (pid < 0)
    {
        perror("fork failed!");
        exit(1);
    }
    else if (pid == 0)
    { // Child process (producer)
        producer_method(shm);
        exit(0);
    }
    else
    { // Parent process (consumer)
        consumer_method(shm);
        wait(NULL); // Wait for child process to finish
    }

    // Unmap the shared memory object from the address space
    if (munmap(shm, DATA_SIZE) != 0)
    {
        perror("munmap failed!");
        exit(1);
    }

    // Remove the shared memory object
    if (shm_unlink(sharedMemoryName) != 0)
    {
        perror("shm_unlink failed");
        exit(1);
    }

    return 0;
}
