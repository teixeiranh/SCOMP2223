#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#define SHM_NAME "/shared_memory"
#define ARRAY_SIZE 10

/* Structure to be shared between the reader and writer. */
typedef struct
{
    int array[ARRAY_SIZE];
    int ready; // used for syncronization
} ShrMemory;

/* Main function to implement the reader for ex03 of PL3. */
int main()
{
    const int DATA_SIZE = sizeof(ShrMemory);
    // Open the shared memory object
    int shareMemoryFiledes = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    // O_RDWR : Open for reading and writing
    // 0644 : Permission for owner (6 - read and write), group (4 - read), and others (4 - read)
    if (shareMemoryFiledes == -1)
    {
        perror("shm_open");
        exit(1);
    }

    // Map the shared memory object
    ShrMemory *shared = (ShrMemory *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shareMemoryFiledes, 0);
    if (shared == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    // Actively wait for the 'ready' flag to be set by the writer process
    while (!shared->ready)
    {
        usleep(1000); // avoid program blocking
    }

    // Calculate the average of the numbers in the shared memory and print it
    int sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        sum += shared->array[i];
    }
    double average = (double)sum / ARRAY_SIZE;
    printf("The average is: %f\n", average);

    // Unmap the shared memory object
    if (munmap(shared, DATA_SIZE) == -1)
    {
        perror("munmap");
        exit(1);
    }

    // Close and unlink the shared memory object
    close(shareMemoryFiledes);
    shm_unlink(SHM_NAME);

    return 0;
}
