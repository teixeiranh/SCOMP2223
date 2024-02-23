#include <stdio.h>     // used to get input and output
#include <unistd.h>    // used to get access to the system calls
#include <fcntl.h>     // used to get access to the system calls
#include <sys/mman.h>  // used to get mmap() and munmap()
#include <sys/stat.h>  // used to get shm_open() and shm_unlink()
#include <sys/types.h> // used to get shm_open() and shm_unlink()
#include <stdlib.h>    // used to get exit()

#define SHM_NAME "/shared_memory"
#define ARRAY_SIZE 10
#define SEQUENCES 5

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

    int counter = 0;
    do {
        while(shared->ready==0); /* espera ativa enquanto o leitor não consumir */

        int sum = 0;
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            sum += shared->array[i];
        }
        double average = (double)sum / ARRAY_SIZE;
        printf("The average is: %f\n", average);
        shared->ready = 0; 
        ++counter;
    } while(counter < SEQUENCES);

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
