#include <fcntl.h>     // Defines the file descriptor
#include <stdio.h>     // Defines the standard input/output functions
#include <stdlib.h>    // Defines the standard library functions
#include <string.h>    // Defines the string functions
#include <sys/mman.h>  // Defines the memory map functions
#include <sys/stat.h>  // Defines the structure for storing file information
#include <sys/types.h> // Defines the type definitions
#include <sys/wait.h>  // Defines the wait functions
#include <unistd.h>    // Defines the symbolic constants and types

/* Constants to be used in the code */
#define BUFFER_SIZE 10 // When bigger than 10, the producer will wait for space in the buffer
#define NUM_VALUES 30  // Total number of values to exchange between producer and consumer

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

#define SHM_NAME "/myshm"
#define SHM_SIZE sizeof(sharedMemory)

/* Struct to store the shared memory data */
typedef struct {
    int numberOfElements;    // Number of elements currently in the buffer
    int readIndex;           // Index for reading
    int writeIndex;          // Index for writing
    int buffer[BUFFER_SIZE]; // Circular buffer for storing integers
} sharedMemory;


/* Main function to solve ex10 of PL3. */
int main() {
    int producer_to_consumer_pipe[2], consumer_to_producer_pipe[2];

    // create pipes for synchronization
    if (pipe(producer_to_consumer_pipe) == -1 || pipe(consumer_to_producer_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create shared memory object
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // Open a shared memory object
    if (fd == -1) {
        perror("shm_open error!");
        exit(EXIT_FAILURE);
    }
    
    // Set the size of the shared memory object
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate error!");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object into the address space
    sharedMemory *buffer = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // NULL indicates that the kernel chooses the address at which to map the file
    if (buffer == MAP_FAILED) {
        perror("mmap error!");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        perror("fork failed!");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process (producer)
        close(producer_to_consumer_pipe[PIPE_READ_END]); // close unused read end of producer pipe
        close(consumer_to_producer_pipe[PIPE_WRITE_END]); // close unused write end of consumer pipe
        
        for (int i = 0; i < NUM_VALUES; ++i) {
            while (buffer->numberOfElements == BUFFER_SIZE) {
                read(consumer_to_producer_pipe[PIPE_READ_END], &buffer->numberOfElements, sizeof(buffer->numberOfElements)); // wait passively    
            }

            buffer->buffer[buffer->writeIndex] = i;                      // Write the current value into the buffer
            buffer->writeIndex = (buffer->writeIndex + 1) % BUFFER_SIZE; // Move write index to next position
            buffer->numberOfElements++;                                  // Increment the number of elements in the buffer

            // signal the consumer that there is a new element to consume
            write(producer_to_consumer_pipe[PIPE_WRITE_END], &buffer->numberOfElements, sizeof(buffer->numberOfElements));
        }

        close(producer_to_consumer_pipe[PIPE_WRITE_END]); // close write end of producer pipe
        close(consumer_to_producer_pipe[PIPE_READ_END]); // close write read of consumer pipe

        exit(EXIT_SUCCESS);
    } else { // Parent process (consumer)
        close(producer_to_consumer_pipe[PIPE_WRITE_END]); // close unused write end of producer pipe
        close(consumer_to_producer_pipe[PIPE_READ_END]); // close unused read end of consumer pipe

        for (int i = 0; i < NUM_VALUES; ++i)
        {
            // wait for there to be elements in the buffer
            while (buffer->numberOfElements == 0) {
                read(producer_to_consumer_pipe[PIPE_READ_END], &buffer->numberOfElements, sizeof(buffer->numberOfElements)); // wait passively
            }

            printf("Consumer read value: %d\n", buffer->buffer[buffer->readIndex]); // Print the value read from the buffer
            buffer->readIndex = (buffer->readIndex + 1) % BUFFER_SIZE;              // Move read index to next position
            buffer->numberOfElements--;                                             // Decrement the number of elements in the buffer
        
            // signal the producer that there is room for more elements
            write(consumer_to_producer_pipe[PIPE_WRITE_END], &buffer->numberOfElements, sizeof(buffer->numberOfElements));
        }
        close(producer_to_consumer_pipe[PIPE_READ_END]); // close read end of producer pipe
        close(consumer_to_producer_pipe[PIPE_WRITE_END]); // close write end of consumer pipe

        wait(NULL); // Wait for child process to finish
    }

    // Unmap the shared memory object from the address space
    if (munmap(buffer, SHM_SIZE) == -1) {
        perror("munmap failed!");
        exit(EXIT_FAILURE);
    }

    // Close shared memory object
    if (close(fd) == -1) {
        perror("close fd failed");
        exit(EXIT_FAILURE);
    }

    // Remove the shared memory object
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
