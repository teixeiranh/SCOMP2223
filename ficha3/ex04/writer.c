#include <stdio.h>     // used to get printf()
#include <stdlib.h>    // used to get exit()
#include <time.h>      // used to get time()
#include <unistd.h>    // used to get sleep()
#include <fcntl.h>     // used to get O_CREAT, O_RDWR, O_TRUNC, O_EXCL
#include <sys/mman.h>  // used to get mmap() and munmap()
#include <sys/stat.h>  // used to get sizeof()
#include <sys/types.h> // used to get sizeof()

#define SHM_NAME "/shared_memory"
#define ARRAY_SIZE 10
#define SEQUENCES 5

/* Structure to be shared between the reader and writer. */
typedef struct
{
    int array[ARRAY_SIZE];
    int ready;
} ShrMemory;

/* Main function to implement the writer for ex03 of PL3. */
int main()
{
    const int DATA_SIZE = sizeof(ShrMemory);
    // Open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    // O_CREAT - create the shared memory object if it does not exist
    // O_RDWR - open the shared memory object for reading and writing
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory object, using the size of the structure
    if (ftruncate(shm_fd, DATA_SIZE) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory object
    ShrMemory *shared = (ShrMemory *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // PROT_READ - the shared memory object can be read
    // PROT_WRITE - the shared memory object can be written
    // MAP_SHARED - the shared memory object can be shared between processes
    if (shared == MAP_FAILED) // MAP_FAILED is defined in <sys/mman.h>
    {
        perror("mmap");
        exit(1);
    }

	srand(time(NULL));
    int counter = 0;
    do {
        while(shared->ready==1); /* espera ativa enquanto o leitor não consumir */

        for (int i = 0; i < ARRAY_SIZE; i++) {
            shared->array[i] = rand() % 20 + 1; // numbers from 1 to 20
        }
        shared->ready = 1; 
        ++counter;
    } while(counter < SEQUENCES);
    
    // Unmap the shared memory object
    if (munmap(shared, DATA_SIZE) == -1)
    {
        perror("munmap");
        exit(1);
    }

    // Close the shared memory object
    close(shm_fd);

    return 0;
}
