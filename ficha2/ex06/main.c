/* Ex6.
Given two integer arrays vec1 and vec2, with 1000 elements each, write a program that creates 5 child processes
that communicate with the parent through a pipe, to concurrently sum the two arrays. Each child should calculate
tmp += vec1[i] + vec2[i] on 200 elements, then sending tmp to its parent through the pipe. Ensure that each child
computes different positions of the arrays. The parent process should wait for the 5 partial sums and then calculate
the final result. There must be only one pipe, shared by all processes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
#define CHUNK_SIZE 200
#define NUM_CHILDREN 5

int main() {
    int vec1[ARRAY_SIZE];
    int vec2[ARRAY_SIZE];

    // Initialize arrays with random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        vec1[i] = rand();
        vec2[i] = rand();
    }

    int fd[2]; // Pipe for child to parent communication

    // Create the pipes
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create the child processes
    pid_t pid;
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(fd[0]); // Close read end of pipe

            // Compute the sum of 200 elements from vec1 and vec2
            int start = i * CHUNK_SIZE;
            int end = start + CHUNK_SIZE;
            int tmp = 0;
            for (int j = start; j < end; j++) {
                tmp += vec1[j] + vec2[j];
            }

            // Send the sum to the parent process
            if (write(fd[1], &tmp, sizeof(tmp)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            close(fd[1]); // Close write end of pipe
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for the child processes to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Parent process
    close(fd[1]); // Close write end of pipe

    int sum = 0;
    int tmp = 0;

    // Read the partial sums from the child processes and accumulate them
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (read(fd[0], &tmp, sizeof(tmp)) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        sum += tmp;
    }

    printf("The sum of the two vecs is %d\n", sum);

    close(fd[0]); // Close read end of pipe

    exit(EXIT_SUCCESS);

    return 0;
}

/*
make run
gcc -Wall -g -c main.c
gcc -Wall -g -o ex06 main.o
./ex06
The sum of the two vecs is 306087076
*/

/*
a. Explain why the there is no need for a synchronization mechanism? Take a look at the pipe function
complete man pages.

The pipe() function creates a unidirectional communication channel between two processes, where one 
process can write to the pipe and the other process can read from the pipe. When one process writes 
to the pipe, the data is stored in a buffer and is not immediately sent to the other process. 
Similarly, when one process reads from the pipe, it reads from the buffer and removes the data from 
the buffer.
According to the pipe function manual, "Data can be written to the file descriptor fd[1] and read 
from the file descriptor fd[0]. The read and write operations on the pipe are atomic." This means 
that when a process writes to the write end of the pipe and another process reads from the read end 
of the pipe, the data is transferred atomically, meaning that there is no need for explicit 
synchronization mechanisms such as locks or semaphores. The read and write operations on the pipe 
are guaranteed to be thread-safe, so multiple processes can access the pipe without interfering with 
each other. Therefore, in the provided code, there is no need for explicit synchronization mechanisms 
as the pipe itself provides the necessary synchronization.
*/